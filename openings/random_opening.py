import argparse
import json
import os
import random
from dataclasses import dataclass, field


@dataclass
class position:
    board: list[list[int]] = field(
            default_factory=lambda: [[0 for j in range(6)] for i in range(7)])
    color: int = 1


def leagal_moves(pos):
    moves = []
    for i in range(7):
        for j in range(6):
            if pos.board[i][j] == 0:
                moves.append([i, j])
                # if you can play at the position you don't have to look at the upper
                break
    return moves


def is_won(pos):
    # horizontal -
    for i in range(4):
        for j in range(6):
            if (pos.board[i+0][j] ==
                    pos.board[i+1][j] ==
                    pos.board[i+2][j] ==
                    pos.board[i+3][j] != 0):
                return True

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


def do_move(pos, mv):
    pos.board[mv[0]][mv[1]] = pos.color
    pos.color *= -1
    return pos


def pos_to_LN(pos):
    position_LN = ""

    # convert the position to LN
    for j in range(6):
        for i in range(7):
            if pos.board[i][j] == 1:
                position_LN += "R"
            elif pos.board[i][j] == -1:
                position_LN += "Y"
            else:
                position_LN += "_"

    position_LN += " "

    # color to move
    if pos.color == 1:
        position_LN += "R"
    else:
        position_LN += "Y"

    return position_LN


def random_opening(args):
    pos = position()
    found = False
    while not found:
        pos = position()
        for i in range(args.depth):
            mv = random.choice(leagal_moves(pos))
            pos = do_move(pos, mv)
        found = not is_won(pos)
    return pos_to_LN(pos)


def write_to_JSON(args, data):
    with open(args.savefile, "w", encoding="utf-8") as file:
        json.dump(data, file, ensure_ascii=False, indent=4)


def exist_file(args):
    if os.path.exists(args.savefile):
        print("File already exists.")
        quit()


def main(args):
    exist_file(args)
    openings = []
    for i in range(args.position):
        openings.append(random_opening(args))
        if (i % 1000 == 999):
            print((i+1), (args.position))
    write_to_JSON(args, openings)


def check_positive(value):
    if not value.isdigit() or int(value) == 0:
        raise argparse.ArgumentTypeError("invalit positive int value: %s" % value)
    return int(value)


def check_json(file):
    if len(file) <= 5:
        raise argparse.ArgumentTypeError("invalit json file. To short: %s" % file)
    if file[-5:] != ".json":
        raise argparse.ArgumentTypeError("invalit json file. Wrong ending: %s" % file)
    return file


if __name__ == "__main__":
    # parser
    parser = argparse.ArgumentParser(
            prog="random opening",
            description="list of random position with a defined depth of the starting position")
    # arguments
    parser.add_argument("-d", "--depth", default=5, type=check_positive,
            help="number of moves from the starting position")
    parser.add_argument("-p", "--position", default=1000, type=check_positive,
            help="number of opening positions")
    parser.add_argument("--savefile", default="default.json", type=check_json)
    args = parser.parse_args()

    if args.savefile == "default.json":
        args.savefile = "random_opening_" + str(args.position) + "-" + str(args.depth) + ".json"

    main(args)
