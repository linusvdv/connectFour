import argparse
import asyncio
import json
import os
from dataclasses import dataclass, field
from queue import Queue
from threading import Thread


@dataclass
class position:
    board: list[list[int]] = field(
            default_factory=lambda: [[0 for j in range(6)] for i in range(7)])
    color: int = 1


game_res = [0, 0, 0]


def debug(args, player, pos, thread_number):
    if args in ["All"]:
        # print(thread_number, "\t" + args[("program_" + str(player+1))], pos)
        print(thread_number, "\t" + pos)


async def clear_buffer(args, protocol, running, player, pos, thread_number):
    # check if the buffer has still data in it
    while running[player].done():
        if args.debug in ["Warning", "Info", "All"]:
            print(thread_number, "\tWARNING: still data in the buffer:", get_res(running[player]))
        debug(args, player, pos, thread_number)
        running[player] = create_running(protocol[player])


def get_res(running):
    return running.result().decode('ascii').rstrip()


async def get_data(protocol):
    return await protocol.stdout.readline()


def create_running(protocol):
    return asyncio.create_task(get_data(protocol))


async def send_message(args, protocol, running, player, p_send, p_expect, p_error, pos, thread_number):
    # check if the buffer has still data in it
    await clear_buffer(args, protocol, running, player, pos, thread_number)

    # send the message
    protocol[player].stdin.write(f'{p_send}\n'.encode('ascii'))
    # get the result
    await running[player]
    if get_res(running[player]) != p_expect:
        # error handling
        print(thread_number, "\t" + str(player) + ":", "ERROR:", p_error)
        if args.debug in ["Error", "Warning", "Info", "All"]:
            print(thread_number, "\t" + str(player) + ":", "send:    ", p_send)
            print(thread_number, "\t" + str(player) + ":", "expected:", p_expect)
            print(thread_number, "\t" + str(player) + ":", "got:     ", get_res(running[player]))
        debug(args, player, pos, thread_number)
    else:
        # normal procedure
        if args.debug in ["Info", "All"]:
            print(thread_number, "\t" + str(player) + ":", get_res(running[player]))
        debug(args, player, pos, thread_number)

    # start running again
    running[player] = create_running(protocol[player])


async def search(args, protocol, running, player, thread_number):
    # writes to the program
    command = "go depth " + str(args.depth)
    protocol[player].stdin.write(f'{command}\n'.encode('ascii'))

    # awaits until the it gets the output
    is_info = True
    out = ""
    while is_info:
        await running[player]
        out = get_res(running[player])

        # ignore inputs that begin with a info
        if len(out) > 4 and out[:4] != "info":
            if (len(out) > 6 and out[:6] == "depth:"):
                is_info = False
                if args.debug in ["Info", "All"]:
                    print(thread_number, "\t" + str(player) + ":", out)
            else:
                if args.debug in ["Warning", "Info", "All"]:
                    print(thread_number, "\t" + str(player) + ":", out)

        else:
            if args.debug in ["Info", "All"]:
                print(thread_number, "\t" + str(player) + ":", out)

        running[player] = create_running(protocol[player])

    return out


def leagal_moves(pos):
    moves = []
    for i in range(7):
        for j in range(6):
            if pos.board[i][j] == 0:
                moves.append(j*7+i)
                break
    return moves


def is_leagal(pos, move):
    if move in leagal_moves(pos):
        return True
    return False


def is_won(pos):
    # horizontal -
    for i in range(4):
        for j in range(6):
            if (pos.board[i+0][j] ==
                    pos.board[i+1][j] ==
                    pos.board[i+2][j] ==
                    pos.board[i+3][j] != 0):
                return pos.board[i][j]

    # vertical |
    for i in range(7):
        for j in range(3):
            if (pos.board[i][j+0] ==
                    pos.board[i][j+1] ==
                    pos.board[i][j+2] ==
                    pos.board[i][j+3] != 0):
                return pos.board[i][j]

    # diagonal \
    for i in range(4):
        for j in range(3):
            if (pos.board[i+0][j+0] ==
                    pos.board[i+1][j+1] ==
                    pos.board[i+2][j+2] ==
                    pos.board[i+3][j+3] != 0):
                return pos.board[i][j]

    # diagonal /
    for i in range(4):
        for j in range(3):
            if (pos.board[i+0][j+3] ==
                    pos.board[i+1][j+2] ==
                    pos.board[i+2][j+1] ==
                    pos.board[i+3][j+0] != 0):
                return pos.board[i][j+3]


def get_pos(opening, thread_number):
    pos = position()

    if len(opening) != 44:
        print(thread_number, "\t" + opening)
        quit()

    for j in range(6):
        for i in range(7):
            if opening[0] == "R":
                pos.board[i][j] = 1
            elif opening[0] == "Y":
                pos.board[i][j] = -1
            else:
                pos.board[i][j] = 0
            opening = opening[1:]

    if opening[1] == "R":
        pos.color = 1
    else:
        pos.color = -1

    return pos


def stones(pos):
    num = 0
    for i in range(7):
        for j in range(6):
            if pos.board[i][j] != 0:
                num += 1
    return num


async def game_handler(args, protocol, running, start_color, opening, thread_number):
    # position
    pos = get_pos(opening, thread_number)

    pos_str = "position LN " + opening

    til = 42 - stones(pos)
    for i in range(til):
        player = (start_color + i) % 2

        # check if the program is ready
        await send_message(args, protocol, running, player,
            "isready", "readyok", "not ready", pos, thread_number)

        # set the position
        await send_message(args, protocol, running, player,
            pos_str, "Set the position LN", "while setting the position", pos, thread_number)

        # analyse the position
        out = await search(args, protocol, running, player, thread_number)

        # get the move
        move = int(out.split()[3])

        # check if the move is leagal
        if not is_leagal(pos, move):
            if args.debug in ["Error", "Warning", "Info", "All"]:
                print(str(player)+":", "Illeagal move")
                print(thread_number, "\t", pos_str)
                print(til, i)
            if pos.color == -1:
                return (pos_str, [1, 0, 0])
            elif pos.color == 1:
                return (pos_str, [0, 0, 1])

        # do the move
        x, y = move % 7, move // 7
        pos.board[x][y] = pos.color
        pos.color *= -1

        if i == 0:
            pos_str += " moves"
        pos_str += " "
        pos_str += str(move)

        # check if the position has a connect four
        if is_won(pos) == 1:
            return (pos_str, [1, 0, 0])
        elif is_won(pos) == -1:
            return (pos_str, [0, 0, 1])

    return (pos_str, [0, 1, 0])


async def newgame(args, protocol, running, thread_number):
    # protocol
    for player in range(2):
        await send_message(args, protocol, running, player,
            "newgame", "started a new game", "starting a new game", "", thread_number)


async def game(args, q_openings: Queue, q_games: Queue, thread_number):
    # create the protocol
    protocol = [
        await asyncio.create_subprocess_exec(
            "./"+args.program_1,
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        ),
        await asyncio.create_subprocess_exec(
            "./"+args.program_2,
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
    ]

    # Start the subprocess and check if they start correctly
    running = [create_running(protocol[0]),
               create_running(protocol[1])]
    await running[0]
    await running[1]
    if get_res(running[0]) == \
       get_res(running[1]) == \
       "Connect Four by linusvdv":
        if args.debug in ["Info", "All"]:
            print(thread_number, "\tStarted both programs")
    else:
        if args.debug in ["Error", "Warning", "Info", "All"]:
            print(thread_number, "\tERROR: started incorrectly", get_res(running[0]), get_res(running[1]))
        quit()

    # Starting the subprocess for playing
    running = [create_running(protocol[0]),
               create_running(protocol[1])]

    while q_openings.qsize() > 0:
        # get opening
        opening = q_openings.get()

        game_handler_res = await game_handler(args, protocol, running, 0, opening, thread_number)
        game_res[0] += game_handler_res[1][0]
        game_res[1] += game_handler_res[1][1]
        game_res[2] += game_handler_res[1][2]
        if not args.debug == "None":
            print("W:", game_res[0], "D:", game_res[1], "L:", game_res[2])
        await newgame(args, protocol, running, thread_number)
        q_games.put(game_handler_res)

        if args.save is False:
            game_handler_res = await game_handler(args, protocol, running, 1, opening, thread_number)
            game_res[0] += game_handler_res[1][2]
            game_res[1] += game_handler_res[1][1]
            game_res[2] += game_handler_res[1][0]
            if not args.debug == "None":
                print("W:", game_res[0], "D:", game_res[1], "L:", game_res[2])
            await newgame(args, protocol, running, thread_number)
            q_games.put(game_handler_res)


# sync function to call a async function
def game_thread(args, q_openings: Queue, q_games: Queue, thread_number):
    asyncio.run(game(args, q_openings, q_games, thread_number))


# get the opening book
def read_from_JSON(args):
    with open(args.openingbook, "r", encoding="utf-8") as file:
        return json.load(file)


# write to the savefile
def write_to_JSON(args, data):
    with open(args.savefile, "w", encoding="utf-8") as file:
        json.dump(data, file, ensure_ascii=False, indent=4)


def main(args):
    q_openings = Queue()
    q_games = Queue()
    gamethreads = []

    for opening in read_from_JSON(args):
        q_openings.put(opening)

    for i in range(args.threads):
        gamethreads.append(Thread(target=game_thread, args=(args, q_openings, q_games, i)))
        gamethreads[i].start()

    # wait until threads are finished
    for i in range(args.threads):
        gamethreads[i].join()

    l_games = []
    while q_games.qsize() > 0:
        l_games.append(q_games.get())
    write_to_JSON(args, l_games)


def check_executable_file(file):
    if not os.path.exists(file):
        raise argparse.ArgumentTypeError("invalid file: %s" % file)
    if not os.access(file, os.X_OK):
        raise argparse.ArgumentTypeError("file %s not executable" % file)
    return file


def check_positive(value):
    if not value.isdigit() or int(value) == 0:
        raise argparse.ArgumentTypeError("invalid positive int value: %s" % value)
    return int(value)


def argparsebool(value):
    if value in ["True", "False"]:
        return bool(value)
    else:
        raise argparse.ArgumentTypeError("invalid choice: %s (choose from 'True', 'False')" % value)


def check_json(file):
    if not os.path.exists(file):
        raise argparse.ArgumentTypeError("invalid file: %s" % file)
    return check_new_json(file)


def check_new_json(file):
    if len(file) <= 5:
        raise argparse.ArgumentTypeError("invalid json file. To short: %s" % file)
    if file[-5:] != ".json":
        raise argparse.ArgumentTypeError("invalid json file. Wrong ending: %s" % file)
    return file


if __name__ == "__main__":
    # arguments
    parser = argparse.ArgumentParser(
            prog="playtest",
            description="plays two connect four program against each other.",
            epilog="python3 ...")
    # necessary arguments
    parser.add_argument("program_1", type=check_executable_file, help="file of program 1")
    parser.add_argument("program_2", type=check_executable_file, help="file of program 2")
    # options
    parser.add_argument("-t", "--threads", default=1, type=check_positive, help="nuber of threads used")
    parser.add_argument("-d", "--depth", default=5, type=check_positive, help="depth of search")
    parser.add_argument("-g", "--games", default=1, type=check_positive, help="numbers of gamepairs")
    parser.add_argument("-s", "--save", default="False", type=argparsebool,
            help="save the game to a file and its outcome")
    parser.add_argument("--debug", default="Warning", type=str,
            choices=["None", "Error", "Warning", "Info", "All"], help="level of debug")
    # file names
    parser.add_argument("--openingbook", default="../openings/random_opening_10000-6.json",
            type=check_json, help="file of openings")
    parser.add_argument("--savefile", default="games.json", type=check_new_json, help="file to save the games to")
    args = parser.parse_args()

    # using asyncio to run async processes
    main(args)
