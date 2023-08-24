import argparse
import os


def main(args):
    return


def check_executable_file(file):
    if not os.path.exists(file):
        raise argparse.ArgumentTypeError("invalit file: %s" % file)
    if not os.access(file, os.X_OK):
        raise argparse.ArgumentTypeError("file %s not executable" % file)
    return file


def check_positive(value):
    if not value.isdigit() or int(value) == 0:
        raise argparse.ArgumentTypeError("invalit positive int value: %s" % value)
    return int(value)


def argparsebool(value):
    return bool(value)


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
    parser.add_argument("-t", "--treads", default=1, type=check_positive, help="nuber of threads used")
    parser.add_argument("-d", "--depth", default=5, type=check_positive, help="depth of search")
    parser.add_argument("-g", "--games", default=1, type=check_positive, help="numbers of gamepairs")
    parser.add_argument("-s", "--save", default="True", type=argparsebool,
            choices=["True", "False"], help="save the game to a file and its outcome")
    parser.add_argument("--debug", default="Warning", type=str,
            choices=["None", "Error", "Warning", "Info", "All"], help="level of debug")
    # file names
    parser.add_argument("--savefile", default="games.json")
    args = parser.parse_args()

    # using asyncio to run async processes
    main(args)
