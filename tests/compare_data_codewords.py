import gdb
import sys
import os


# Add in tests/ directory to path for imports
test_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, test_dir)

from constants import DEBUG_TARGET, QR_MASK, DEFAULT_ERROR
from util import get_encoding, make, segno_make_qr_replicated, print_data_diffs

# Have to use env cuz no args to gdb run python scripts afaik
exec_args = [f'"{os.environ.get("ARG_ONE", "")}"']

if os.environ.get("ARG_TWO"):
    exec_args.append(f'"{os.environ["ARG_TWO"]}"')


def execute(cmd):
    return gdb.execute(cmd, to_string=True)


def get_data_codeword_bits():
    execute(f"file {DEBUG_TARGET}")
    execute(f"set args {' '.join(exec_args)}")
    print(f"set args {' '.join(exec_args)}")

    execute("break encode_into_codewords")

    execute("run")
    execute("finish")

    cw_len = int(execute("p/d version.capacity").split("=")[1].strip())

    codewords = []
    for i in range(cw_len):
        b_str = execute(f"p/t codewords[{i}]")
        codewords.append(int(b_str.split("=")[1].strip(), base=2))

    cw_bits = []
    for cw in codewords:
        for c in "{:08b}".format(cw):
            cw_bits.append(1 if c == "1" else 0)

    return bytearray(cw_bits)


def main():
    make()

    cw_bits = get_data_codeword_bits()

    if len(exec_args) == 1:
        content = bytes(exec_args[0][1:-1], encoding="ascii")
    else:
        with open(exec_args[1][1:-1], "rb") as f:
            content = f.read()

    segno_cw_bits = segno_make_qr_replicated(
        content,
        error=DEFAULT_ERROR,
        mask=QR_MASK,
        boost_error=False,
        return_data=True,
        encoding=get_encoding(content),
        micro=False,
    )

    print_data_diffs(cw_bits, segno_cw_bits, forced_bytes=[2360, 2479, 2598])

    execute("quit")


if __name__ == "__main__":
    main()
