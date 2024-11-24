import argparse
import os
import subprocess

SYMBOL_SIZE_COMMAND = ["nm", "-n", "--print-size"]


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "object_file",
        type=str,
        help="Object file with symbol table data",
    )
    args = parser.parse_args()
    obj_file = args.object_file

    if not os.path.exists(obj_file):
        print(f'File "{obj_file}" doesn\'t exist')
        exit(1)

    return obj_file


def get_symbol_table(object_file):
    symbol_process = subprocess.run(
        SYMBOL_SIZE_COMMAND + [object_file], capture_output=True, text=True
    )

    return symbol_process.stdout


def print_symbol_sizes(symbol_table: str):

    def get_size(s):
        return int(s.split(" ")[1])

    res_lines = []

    for line in symbol_table.split("\n"):
        if not line:
            continue
        line_data = line.split(" ")
        name = line_data[-1]
        size = int(line_data[1], base=16)
        res_lines.append(
            f"{name}: {size} ({'0x{:0x}'.format(size)}) byte{'s' if size != 1 else ''}"
        )

    res_lines.sort(key=get_size)

    print("\n".join(res_lines))


def main():
    object_file = parse_args()
    symbol_table = get_symbol_table(object_file)
    print_symbol_sizes(symbol_table)


if __name__ == "__main__":
    main()
