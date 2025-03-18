import argparse
import pathlib
import os

ROOT = pathlib.Path(__file__).parent.parent
DIR_STATS = dict()
FILE_STATS = dict()


def file_filter(entry, exts):
    for ext in exts:
        if ext in entry:
            return True
    return False


def visit(directory, exts):
    loc = 0

    for entry in os.listdir(directory):
        if os.path.isfile(directory / entry) and file_filter(entry, exts):
            with open(directory / entry, "r") as file:
                file_loc = sum(1 for _ in file)
            loc += file_loc
            FILE_STATS[str(entry)] = file_loc
        if os.path.isdir(directory / entry):
            loc += visit(directory / entry, exts)

    DIR_STATS[str(directory.relative_to(ROOT))] = loc
    return loc


def main():
    parser = argparse.ArgumentParser("Count `loc` in directory recursively")
    parser.add_argument(
        "--dir",
        help="directory to count loc",
        default="./engine/code,./engine/plugins,./editor/code,./editor/plugins",
    )
    parser.add_argument(
        "--ext", help="filter for file extensions", default="cpp,hpp,h,glsl"
    )
    args = parser.parse_args()
    exts = set(["." + ext for ext in args.ext.split(",")])
    dirs = str(args.dir).split(",")

    loc_total = 0

    for d in dirs:
        loc_total = loc_total + visit(ROOT / d, exts)

    print("files loc:")
    for file_name, loc in FILE_STATS.items():
        print(f" - {file_name} loc {loc}")

    print("dirs loc:")
    for dir_name, loc in DIR_STATS.items():
        print(f" - {dir_name} loc {loc}")

    print(f"total loc {loc_total}")


if __name__ == "__main__":
    main()
