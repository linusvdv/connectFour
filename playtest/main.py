import asyncio
import sys
import numpy as np


async def get_data(protocol):
    return await protocol.stdout.readline()


def get_res(running):
    return running.result().decode('ascii').rstrip()


def create_running(protocol):
    return asyncio.create_task(get_data(protocol))


async def clear_buffer(protocol, running, player):
    # check if the buffer has still data in it
    while running[player].done():
        print("WARNING: still data in the buffer:",
              get_res(running[player]))
        running[player] = create_running(protocol[player])


async def isready(protocol, running, player):
    # check if the program is read
    protocol[player].stdin.write(b"isready\n")
    await running[player]
    if not get_res(running[player]) == "readyok":
        print("ERROR:", player, "isready:", get_res(running[player]))
    else:
        print(player, get_res(running[player]))
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

    if color == 1:
        protocol += "R"
    else:
        protocol += "Y"

    protocol += "\n"

    return protocol.encode('ascii')


def is_won(position):
    # horizontal
    for i in range(4):
        for j in range(6):
            if (position[i+0][j] ==
                position[i+1][j] ==
                position[i+2][j] ==
                position[i+3][j] != 0):
                return position[i][j]

    # vertical
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


async def game(protocol, running, start_color):
    # position
    position = [[0 for j in range(6)] for i in range(7)]
    color = 1

    for i in range(48):
        player = (start_color + i) % 2

        # check if the buffer has still data in it
        await clear_buffer(protocol, running, player)

        # check if the program is ready
        await isready(protocol, running, player)

        # playing the game
        # set the position
        protocol[player].stdin.write(position_LN(position, color))
        await running[player]
        if (get_res(running[player])[:19] == "Set the position LN"):
            print(player, get_res(running[player]))
        running[player] = create_running(protocol[player])
        await clear_buffer(protocol, running, player)

        # analyse the position
        await isready(protocol, running, player)
        protocol[player].stdin.write(b'go depth 7\n')

        is_info = True
        out = ""
        while is_info:
            await running[player]
            out = get_res(running[player])

            if len(out) > 4 and out[:4] != "info":
                is_info = False

            print(player, out)
            running[player] = create_running(protocol[player])

        pos = int(out.split()[5])
        x, y = pos % 7, pos // 7
        position[x][y] = color
        color *= -1

        if is_won(position) == 1:
            return (1, 0, 0)
        elif is_won(position) == -1:
            return (0, 0, 1)

    return (0, 1, 0)


async def newgame(protocol, running, wdl):
    for i in range(2):
        protocol[i].stdin.write(b'newgame\n')
        await running[i]
        if get_res(running[i]) != "started a new game":
            print("ERROR: staring a new game:", get_res(running[i]))


async def main():
    protocol = [
        await asyncio.create_subprocess_exec(
            sys.argv[1],
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        ),
        await asyncio.create_subprocess_exec(
            sys.argv[2],
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
    ]

    # Start the subprocess and check if they start
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
    # win, draw, loss
    wdl = np.array([0, 0, 0])
    for i in range(4):
        # which instance starts
        wdl += await game(protocol, running, 0)
        await newgame(protocol, running, wdl)
        wdl += list(reversed(await game(protocol, running, 1)))
        await newgame(protocol, running, wdl)
        print(wdl)


if __name__ == "__main__":
    asyncio.run(main())
