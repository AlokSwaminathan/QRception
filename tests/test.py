import argparse
import subprocess
import os
import random
from constants import *
from util import make, generate_bmp


def random_bytes(n: int) -> bytes:
    return bytes([random.randint(0, 255) for _ in range(n)])


def random_alphanumeric(n: int) -> bytes:
    return bytes(random.choices(ALPHANUMERIC_CHARACTERS, k=n))


def parse_bmp(expected_size: int) -> bytes:
    parser_process = subprocess.run(
        QR_PARSER + [TEMP_BMP_FILE], capture_output=True, check=True
    )
    result = parser_process.stdout
    return result[:expected_size]


def test_executable(executable: str, input: bytes) -> tuple[bool, bytes]:
    generate_bmp(executable, input)
    res = parse_bmp(len(input))
    return (res == input, res)


def test(target: str, qr_version: int, ec_level: str, show_data: bool):
    min_size = SIZES[qr_version - 1][ec_level] + 1
    max_size = SIZES[qr_version][ec_level]

    alphanum_data_len = (
        random.randint(min_size + EXTRA_BYTES_PER_MODE, max_size) - EXTRA_BYTES_PER_MODE
    )
    byte_data_len = (
        random.randint(min_size + EXTRA_BYTES_PER_MODE, max_size) - EXTRA_BYTES_PER_MODE
    )

    # Two types, random alphanumeric characters and random bytes
    alphanum_data = random_alphanumeric(alphanum_data_len)
    byte_data = random_bytes(byte_data_len)

    target_type = "(DEBUG) " if "debug" in target else ""

    failure_str = f"{target_type}(FAILURE) {{}} data of length {{}} for version {qr_version} and error correction level {ec_level}"
    success_str = f"{target_type}(SUCCESS) {{}} data for version {qr_version} and error correction level {ec_level}"

    alphanum_success, alphanum_res = test_executable(target, alphanum_data)
    byte_success, byte_res = test_executable(target, byte_data)

    if alphanum_success:
        print(success_str.format("alphanumeric"))
    else:
        print(failure_str.format("alphanumeric", str(alphanum_data_len)))
        if show_data:
            print(f"Input: {alphanum_data}")
            print(f"Output: {alphanum_res}")
    if byte_success:
        print(success_str.format("byte"))
    else:
        print(failure_str.format("byte", str(byte_data_len)))
        if show_data:
            print(f"Input: {byte_data}")
            print(f"Output: {byte_res}")

    print()

    return alphanum_success and byte_success


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--show-data", default=False, action="store_true")

    executable_group = parser.add_mutually_exclusive_group(required=False)
    executable_group.add_argument("--executable", help="Custom executable to run")
    executable_group.add_argument(
        "--debug", default=False, action="store_true", help="Use debug executable"
    )

    args = parser.parse_args()

    executable = None
    if args.executable:
        executable = os.path.join(os.getcwd(), args.executable)
        if not os.path.exists(executable):
            print(f"Executable \"{executable} doesn't exist")
            exit(1)
    elif args.debug:
        executable = DEBUG_TARGET
    elif not executable:
        executable = TARGET

    return (executable, args.show_data)


def clean_up_files():
    os.remove(TEMP_DATA_FILE)
    os.remove(TEMP_BMP_FILE)


def main(executable: str, show_data: bool):
    make()
    for version in range(1, MAX_VERSION + 1):
        if not test(executable, version, "L", show_data):
            break


if __name__ == "__main__":
    executable, show_data = parse_args()
    main(executable, show_data)
