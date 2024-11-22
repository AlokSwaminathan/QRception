import gdb
import sys
import os


# Add in tests/ directory to path for imports
test_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, test_dir)

from constants import DEBUG_TARGET, QR_MASK, DEFAULT_ERROR
from util import get_encoding, make, print_data_diffs, segno_make_qr_replicated

# Have to use env cuz no args to gdb run python scripts afaik
exec_args = [f'"{os.environ.get("ARG_ONE", "")}"']

if os.environ.get("ARG_TWO"):
    exec_args.append(f'"{os.environ["ARG_TWO"]}"')


def execute(cmd):
    return gdb.execute(cmd, to_string=True)


def get_final_codeword_bits():
    execute(f"file {DEBUG_TARGET}")
    execute(f"set args {' '.join(exec_args)}")
    print(f"set args {' '.join(exec_args)}")

    execute("break get_full_codewords")

    execute("run")
    execute("finish")

    cw_len_bits = int(execute("p/d version.cw_capacity").split("=")[1].strip()) * 8

    cw_bits = []
    for i in range(cw_len_bits):
        b_str = execute(f"p/t res_bits[{i}]")
        cw_bits.append(int(b_str.split("=")[1].strip(), base=2))

    return bytearray(cw_bits)


def main():
    make()

    final_cw_bits = get_final_codeword_bits()

    if len(exec_args) == 1:
        content = bytes(exec_args[0][1:-1], encoding="ascii")
    else:
        with open(exec_args[1][1:-1], "rb") as f:
            content = f.read()

    segno_final_cw_bits = segno_make_qr_replicated(
        content,
        error=DEFAULT_ERROR,
        mask=QR_MASK,
        boost_error=False,
        return_final_codewords=True,
        encoding=get_encoding(content),
        micro=False,
    )

    print_data_diffs(final_cw_bits, segno_final_cw_bits)

    execute("quit")


if __name__ == "__main__":
    main()
