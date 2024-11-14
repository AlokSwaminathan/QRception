import os
import subprocess
from constants import *


def generate_bmp(executable: str, input: bytes):
    with open(TEMP_DATA_FILE, "wb") as f:
        f.write(input)

    generator_process = subprocess.run(
        [executable, "", TEMP_DATA_FILE], capture_output=True, check=True
    )
    bmp = generator_process.stdout

    with open(TEMP_BMP_FILE, "wb") as f:
        f.write(bmp)


def make():
    make_process = subprocess.run(["make", "-C", PROJECT_ROOT], capture_output=True)
    ret_code = make_process.returncode
    if ret_code != 0:
        print(f"Make failed with a return code of {ret_code}")
        print(f"Stdout:\n{make_process.stdout}")
        print(f"Stderr:\n{make_process.stderr}")
    else:
        print("Sucessfully compiled executables")
