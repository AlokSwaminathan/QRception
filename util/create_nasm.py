import argparse
from os import path
import re
import sys
from typing import Tuple

TAB_LENGTH = 2

TEMPLATE_ELF_FILE = path.join(path.dirname(__file__), "elf_template.asm")

with open(TEMPLATE_ELF_FILE, mode="r") as f:
    template_elf_asm = f.read()


def parse_args() -> Tuple[str, str, str]:
    parser = argparse.ArgumentParser()
    parser.add_argument("objconv_asm", help="NASM file created through objconv")
    parser.add_argument(
        "--entrypoint",
        help="Name of the function that will be used as the entry point for the elf file",
        required=True,
    )
    parser.add_argument("--output", help="File to output result to", required=True)

    args = parser.parse_args()

    objconv_asm_path, entrypoint, output_file = (
        args.objconv_asm,
        args.entrypoint,
        args.output,
    )

    error = False

    if " " in entrypoint:
        print("No spaces allowed in entrypoint")
        error = True

    objconv_asm = ""
    if not path.exists(objconv_asm_path):
        print(f'objconv generated ASM file "{objconv_asm_path}" doesn\'t exist')
        error = True
    else:
        with open(objconv_asm_path, mode="r") as f:
            objconv_asm = f.read()

    if error:
        print("Exiting due to errors...")
        sys.exit(1)

    return objconv_asm, entrypoint, output_file


SECTION_RE = r"SECTION \..*?(?:\n(?!.*?:).*?)*(?=\n.+:|$)"
GLOBAL_RE = r"global .+\n"


def strip_elf64_data(asm):
    asm = re.sub(SECTION_RE, "", asm)
    asm = re.sub(GLOBAL_RE, "", asm)
    return asm


def integrate_template(asm, entrypoint):
    template_lines = template_elf_asm.split("\n")
    template_start = "\n".join(template_lines[:-2]).replace("entry_point", entrypoint)
    template_end = "\n".join(template_lines[-2:])

    return f"{template_start}\n{asm}\n{template_end}"


def main():
    objconv_asm, entrypoint, output_file = parse_args()
    final_asm = strip_elf64_data(objconv_asm)
    final_asm = integrate_template(final_asm, entrypoint)

    with open(output_file, mode="w") as o:
        o.write(final_asm)


if __name__ == "__main__":
    main()
