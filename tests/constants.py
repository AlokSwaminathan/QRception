import os
import json

TEST_ROOT = os.path.dirname(__file__)
PROJECT_ROOT = os.path.dirname(TEST_ROOT)

# Paths relative to project root
TARGET = os.path.join(PROJECT_ROOT, "qr_generator")
DEBUG_TARGET = os.path.join(PROJECT_ROOT, "build/debug/qr_generator")

TEMP_BMP_FILE = os.path.join(TEST_ROOT, "tmp.bmp")
TEMP_SVG_FILE = os.path.join(TEST_ROOT, "tmp.svg")
TEMP_DATA_FILE = os.path.join(TEST_ROOT, "data.tmp")

MAX_VERSION = 40
EC_LEVELS = ["L", "M", "Q", "H"]
EXTRA_BYTES_PER_MODE = 3

ALPHANUMERIC_CHARACTERS = rb"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"

QR_PARSER = ["/usr/bin/zbarimg", "-Sbinary", "--raw"]
TEMP_BMP_FILE = os.path.join(TEST_ROOT, "tmp.bmp")
TEMP_DATA_FILE = os.path.join(TEST_ROOT, "data.tmp")

with open(os.path.join(TEST_ROOT, "max_data_codewords.json"), mode="r") as f:
    SIZES = json.load(f)

BMP_PADDING_SIZE = 3
BMP_BLOCK_SIZE = 8

QR_MASK = 0b01

DEFAULT_ERROR = EC_LEVELS[0]
