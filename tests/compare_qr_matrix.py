import argparse
from sys import executable
from PIL import Image
import numpy as np
import segno

from constants import *
from util import make, generate_bmp, get_encoding


def load_bmp_as_matrix(bmp_file):
    image = Image.open(bmp_file)
    # Convert to black and white (1-bit pixels)
    image = image.convert("1")
    matrix = np.array(image, dtype=int)
    return matrix


def extract_bmp_inner_matrix(matrix):
    padding_size = BMP_PADDING_SIZE * BMP_BLOCK_SIZE
    return matrix[padding_size:-padding_size, padding_size:-padding_size]


def convert_bmp_blocks_to_bits(matrix):
    num_blocks = matrix.shape[0] // BMP_BLOCK_SIZE
    bit_matrix = np.zeros((num_blocks, num_blocks), dtype=int)

    for i in range(num_blocks):
        for j in range(num_blocks):
            block = matrix[
                i * BMP_BLOCK_SIZE : (i + 1) * BMP_BLOCK_SIZE,
                j * BMP_BLOCK_SIZE : (j + 1) * BMP_BLOCK_SIZE,
            ]
            # Set the bit as 1 if the block is black
            bit_matrix[i, j] = 1 if np.mean(block) == 1 else 0
            if np.mean(block) != 1 and np.mean(block) != 0:
                print(f"BUGGING for {i}, {j}")

    return bit_matrix


def get_bmp_matrix(executable, data):
    make()
    generate_bmp(executable, data)
    bmp_matrix = load_bmp_as_matrix(TEMP_BMP_FILE)
    bmp_inner_matrix = extract_bmp_inner_matrix(bmp_matrix)
    bmp_bit_matrix = convert_bmp_blocks_to_bits(bmp_inner_matrix)
    return bmp_bit_matrix


def get_segno_matrix(data):
    encoding = get_encoding(data)
    qr = segno.make_qr(
        content=data,
        error=DEFAULT_ERROR,
        boost_error=False,
        mask=QR_MASK,
        encoding=encoding,
    )

    qr.save(TEMP_SVG_FILE)

    return np.logical_not(
        np.vstack([np.frombuffer(r, dtype=np.uint8) for r in qr.matrix])
    )


def parse_args():
    parser = argparse.ArgumentParser(
        description="Compare segno QR code with generated QR code"
    )

    executable_group = parser.add_mutually_exclusive_group(required=False)
    executable_group.add_argument("--executable", help="Custom executable to run")
    executable_group.add_argument(
        "--debug", default=False, action="store_true", help="Use debug executable"
    )

    data_group = parser.add_mutually_exclusive_group(required=True)
    data_group.add_argument("--file", help="Path to file with data")
    data_group.add_argument("data", help="Data to encode into QR code", nargs="?")

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

    if not args.data:
        with open(args.file, "rb") as f:
            args.data = f.read()
    else:
        args.data = bytes(args.data, encoding="ascii")

    return executable, args.data


def main():
    executable, data = parse_args()

    bmp_matrix = get_bmp_matrix(executable, data)
    segno_matrix = get_segno_matrix(data)

    print(f"Dimensions of BMP matrix: {bmp_matrix.shape}")
    print(f"Dimensions of segno matrix: {segno_matrix.shape}")

    if bmp_matrix.shape != segno_matrix.shape:
        print("Different shapes, exiting...")
        return

    differences = np.sum(bmp_matrix != segno_matrix)

    if differences == 0:
        print("The inner BMP and Segno matrices are equal.")
    else:
        print("The inner BMP and Segno matrices are NOT equal.")
        print(f"Number of differing bits: {differences}")

        diff_coords = np.argwhere(bmp_matrix != segno_matrix)
        diff_coords = diff_coords[np.lexsort((diff_coords[:, 0], diff_coords[:, 1]))]

        print("Coordinates of differing bits:")
        for coord in diff_coords:
            print(f"(x={coord[1]}, y={coord[0]})")

    print(f"Number of differing bits: {differences}")


if __name__ == "__main__":
    main()
