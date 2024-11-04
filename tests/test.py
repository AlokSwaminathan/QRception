import argparse
import subprocess
import os
import json
import random


MAX_VERSION = 40
EC_LEVELS = ["L", "M", "Q", "H"]
EXTRA_BYTES_PER_MODE = 3

ALPHANUMERIC_CHARACTERS = rb"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"

TEST_ROOT = os.path.dirname(__file__)
PROJECT_ROOT = os.path.dirname(TEST_ROOT)
QR_PARSER = "/usr/bin/zbarimg"
TEMP_BMP_FILE = os.path.join(TEST_ROOT, "tmp.bmp")
TEMP_DATA_FILE = os.path.join(TEST_ROOT, "data.tmp")

# Paths relative to project root
TARGET = "qr_generator"
DEBUG_TARGET = "build/debug/qr_generator"

with open(os.path.join(TEST_ROOT, "max_data_codewords.json"), mode="r") as f:
    SIZES = json.load(f)


def random_bytes(n: int) -> bytes:
    return bytes([random.randint(0, 255) for _ in range(n)])


def random_alphanumeric(n: int) -> bytes:
    return bytes(random.choices(ALPHANUMERIC_CHARACTERS, k=n))


def parse_bmp() -> bytes:
    parser_process = subprocess.run(
        [QR_PARSER, "--raw", TEMP_BMP_FILE], capture_output=True
    )
    result = parser_process.stdout
    # Remove the 0x0a byte
    result = result[:-1]
    return result


def generate_bmp(executable: str, input: bytes):
    with open(TEMP_DATA_FILE, "wb") as f:
        f.write(input)

    generator_process = subprocess.run(
        [executable, "", TEMP_DATA_FILE], capture_output=True
    )
    bmp = generator_process.stdout

    with open(TEMP_BMP_FILE, "wb") as f:
        f.write(bmp)


def test_executable(executable: str, input: bytes) -> tuple[bool, bytes]:
    generate_bmp(executable, input)
    res = parse_bmp()
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


def make():
    make_process = subprocess.run(["make", "-C", PROJECT_ROOT], capture_output=True)
    ret_code = make_process.returncode
    if ret_code != 0:
        print(f"Make failed with a return code of {ret_code}")
        print(f"Stdout:\n{make_process.stdout}")
        print(f"Stderr:\n{make_process.stderr}")
    else:
        print("Sucessfully compiled executables")


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", default=False, action="store_true")
    parser.add_argument("--show-data", default=False, action="store_true")

    args = parser.parse_args()
    return (args.debug, args.show_data)


def clean_up_files():
    os.remove(TEMP_DATA_FILE)
    os.remove(TEMP_BMP_FILE)


def main(debug: bool, show_data: bool):
    make()
    target = DEBUG_TARGET if debug else TARGET
    target = os.path.join(PROJECT_ROOT, target)
    for version in range(1, MAX_VERSION + 1):
        if not test(target, version, "L", show_data):
            break
    clean_up_files()


if __name__ == "__main__":
    debug, show_data = parse_args()
    main(debug, show_data)
