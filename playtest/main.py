import asyncio
import sys
import numpy as np
import os.path

# options
debug = "Warning"


def change_options(options):
    # all global options
    global debug

    for option in options:
        # debug
        if option[:7] == "-debug=":
            if option[7:] in ["None", "Error", "Warning", "Info", "All"]:
                debug = option[7:]
            else:
                debug = "Warning"
                print("ERROR: Not a correct debug mode")
                print("got:", option[7:])

        # error handling
        else:
            print("ERROR: Not a correct option")
            print("got:", option)


async def clear_buffer(protocol, running, player):
    # check if the buffer has still data in it
    while running[player].done():
        print("WARNING: still data in the buffer:",
              get_res(running[player]))
        running[player] = create_running(protocol[player])


def get_res(running):
    return running.result().decode('ascii').rstrip()


async def get_data(protocol):
    return await protocol.stdout.readline()


def create_running(protocol):
    return asyncio.create_task(get_data(protocol))


async def send_message(protocol, running, player, p_send, p_expect, p_error):
    # check if the buffer has still data in it
    await clear_buffer(protocol, running, player)

    # send the message
    protocol[player].stdin.write(f'{p_send}\n'.encode('ascii'))
    # get the result
    await running[player]
    if get_res(running[player]) != p_expect:
        # error handling
        print(str(player) + ":", "ERROR:", p_error)
        if debug in ["Error", "Warning", "Info", "All"]:
            print(str(player) + ":", "expected:", p_expect)
            print(str(player) + ":", "got:     ", get_res(running[player]))
    else:
        # normal procedure
        if debug in ["All"]:
            print(str(player) + ":", get_res(running[player]))

    # start running again
    running[player] = create_running(protocol[player])


def position_LN(position, color):
    protocol = "position LN "

    # convert the position to LN
    for j in range(6):
        for i in range(7):
            if position[i][j] == 1:
                protocol += "R"
            elif position[i][j] == -1:
                protocol += "Y"
            else:
                protocol += "_"

    protocol += " "

    # which color moves next
    if color == 1:
        protocol += "R"
    else:
        protocol += "Y"

    return protocol


def leagal_moves(position):
    moves = []
    for i in range(7):
        for j in range(6):
            if position[i][j] == 0:
                moves.append(j*7+i)
                break
    return moves


def is_leagal(position, move):
    if move in leagal_moves(position):
        return True
    return False


def is_won(position):
    # horizontal -
    for i in range(4):
        for j in range(6):
            if (position[i+0][j] ==
                position[i+1][j] ==
                position[i+2][j] ==
                position[i+3][j] != 0):
                return position[i][j]

    # vertical |
    for i in range(7):
        for j in range(3):
            if (position[i][j+0] ==
                position[i][j+1] ==
                position[i][j+2] ==
                position[i][j+3] != 0):
                return position[i][j]

    # diagonal \
    for i in range(4):
        for j in range(3):
            if (position[i+0][j+0] ==
                position[i+1][j+1] ==
                position[i+2][j+2] ==
                position[i+3][j+3] != 0):
                return position[i][j]

    # diagonal /
    for i in range(4):
        for j in range(3):
            if (position[i+0][j+3] ==
                position[i+1][j+2] ==
                position[i+2][j+1] ==
                position[i+3][j+0] != 0):
                return position[i][j+3]


async def search(protocol, running, player):
    # writes to the program
    protocol[player].stdin.write(b'go depth 7\n')

    # awaits until the it gets the output
    is_info = True
    out = ""
    while is_info:
        await running[player]
        out = get_res(running[player])

        # ignore inputs that begin with a info
        if len(out) > 4 and out[:4] != "info":
            is_info = False
            if debug in ["Info", "All"]:
                print(str(player)+":", out)
        else:
            if debug in ["All"]:
                print(str(player)+":", out)
        running[player] = create_running(protocol[player])

    return out


async def game(protocol, running, start_color):
    # position
    position = [[0 for j in range(6)] for i in range(7)]
    color = 1

    for i in range(48):
        player = (start_color + i) % 2

        # check if the program is ready
        await send_message(protocol, running, player,
            "isready", "readyok", "not ready")

        # set the position
        await send_message(protocol, running, player,
            position_LN(position, color), "Set the position LN", "while setting the position")

        print(position_LN(position, color))

        # analyse the position
        out = await search(protocol, running, player)

        # get the move
        move = int(out.split()[3])

        # check if the move is leagal
        if not is_leagal(position, move):
            if debug in ["Error", "Warning", "Info", "All"]:
                print(str(player)+":", "Illeagal move")
            if color == -1:
                return (1, 0, 0)
            elif color == 1:
                return (0, 0, 1)

        # do the move
        x, y = move % 7, move // 7
        position[x][y] = color
        color *= -1

        # check if the position has a connect four
        if is_won(position) == 1:
            return (1, 0, 0)
        elif is_won(position) == -1:
            return (0, 0, 1)

    return (0, 1, 0)


async def newgame(protocol, running, wdl):
    # protocol
    for player in range(2):
        await send_message(protocol, running, player,
            "newgame", "started a new game", "starting a new game")

    # print win draw loss
    print("w:"+str(wdl[0])+",", "d:"+str(wdl[1])+",", "l:"+str(wdl[2]))


async def main():
    # arguments
    if len(sys.argv) <= 2:
        print("ERROR: to few arguments")
        quit()
    elif len(sys.argv) > 3:
        change_options(sys.argv[3:])

    # checks if the first two arguments are files
    if not (os.path.exists(sys.argv[1]) and os.path.exists(sys.argv[2])):
        print("ERROR: files don't exist")
        quit()
    # checks if the first two arguments are executables
    if not (os.access(sys.argv[1], os.X_OK) and os.access(sys.argv[2], os.X_OK)):
        print("ERROR: file is not executable")
        quit()

    # create the protocol
    protocol = [
        await asyncio.create_subprocess_exec(
            "./"+str(sys.argv[1]),
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        ),
        await asyncio.create_subprocess_exec(
            "./"+str(sys.argv[2]),
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
        print("Started both programs")
    else:
        print("ERROR: started incorrectly",
              get_res(running[0]), get_res(running[1]))
        quit()

    # Starting the subprocess for playing
    running = [create_running(protocol[0]),
               create_running(protocol[1])]

    # start of the program
    wdl = np.array([0, 0, 0])
    for i in range(1):
        # which instance starts
        wdl += await game(protocol, running, 0)
        await newgame(protocol, running, wdl)

        # gamepair with reversed colors
#        wdl += list(reversed(await game(protocol, running, 1)))
#        await newgame(protocol, running, wdl)


if __name__ == "__main__":
    asyncio.run(main())
