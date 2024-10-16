import argparse
import re

SYMBOL_RE = r"([0-9a-fA-F]+)\s\w\s(\w+)\n"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("symbol_table", type=str)
    parser.add_argument("--file-size", type=int, required=True)
    args = parser.parse_args()
    return (args.symbol_table, args.file_size)


def get_symbol_locs(symbol_table):
    matches = re.findall(SYMBOL_RE, symbol_table)
    matches = [(int(m1, 16), m2) for m1, m2 in matches]
    return matches


def print_symbol_sizes(locs, file_size):
    locs.append((file_size, ""))
    for i, (loc, name) in enumerate(locs[:-1]):
        diff = locs[i + 1][0] - loc
        print(f"{name}: {diff} byte{'s' if diff != 1 else ''}")


def main():
    symbol_table, file_size = parse_args()
    locs = get_symbol_locs(symbol_table)
    print_symbol_sizes(locs, file_size)


if __name__ == "__main__":
    main()
