import argparse
import pathlib
import os

ROOT = pathlib.Path(__file__).parent.parent
DIR_STATS = dict()
FILE_STATS = dict()


def visit(directory):
    loc = 0

    for entry in os.listdir(directory):
        if os.path.isfile(directory / entry):
            with open(directory / entry, "r") as file:
                file_loc = sum(1 for _ in file)
            loc += file_loc
            FILE_STATS[str(entry)] = file_loc
        else:
            loc += visit(directory / entry)

    DIR_STATS[str(directory.name)] = loc
    return loc


def main():
    parser = argparse.ArgumentParser("Count `loc` in directory recursively")
    parser.add_argument("--dir", help="directory to count loc", default="./")
    args = parser.parse_args()
    loc = visit(ROOT / args.dir)

    print("files loc:")
    for file_name, loc in FILE_STATS.items():
        print(f" - {file_name} loc {loc}")

    print("dirs loc:")
    for dir_name, loc in DIR_STATS.items():
        print(f" - {dir_name} loc {loc}")

    print(f"total loc {loc}")


if __name__ == '__main__':
    main()
