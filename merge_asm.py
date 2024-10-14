import argparse
import os
import re
import sys
from typing import List, Tuple

TAB_LENGTH = 2


def parse_args() -> Tuple[List[Tuple[str, str]], str, str, str]:
    parser = argparse.ArgumentParser()
    parser.add_argument("files", nargs="+", help="A list of file names to process")
    parser.add_argument(
        "--entrypoint",
        help="Name of the function that will be used as the entry point for the elf file",
        required=True,
    )
    parser.add_argument(
        "--elf-template",
        help="ASM file for ELF File that will be used as the template for the result",
        required=True,
    )
    parser.add_argument("--output", help="File to output result to", required=True)

    args = parser.parse_args()

    files, entrypoint, template_elf_file, output_file = (
        args.files,
        args.entrypoint,
        args.elf_template,
        args.output,
    )

    error = False

    if " " in entrypoint:
        print("No spaces allowed in entrypoint")
        error = True

    file_data = []

    for file in files:
        if not os.path.exists(file):
            print(f'File "{file}" doesn\'t exist')
            error = True
        else:
            with open(file, mode="r") as f:
                text = f.read()
            file_data.append((file, text))

    template_elf_asm = ""
    if not os.path.exists(template_elf_file):
        print(f'ELF ASM file "{template_elf_file}" doesn\'t exist')
        error = True
    else:
        with open(template_elf_file, mode="r") as f:
            template_elf_asm = f.read()

    if error:
        print("Exiting due to errors...")
        sys.exit(1)

    return file_data, entrypoint, template_elf_asm, output_file


GCC_SYMBOL_RE = r"\t\.type\t(\w+), @(\w+)\n"


def get_symbols(text: str) -> List[Tuple[str, str]]:
    matches = re.findall(GCC_SYMBOL_RE, text)
    return [(match[0], match[1]) for match in matches]


GCC_STANDARD_SYMBOL_RE = r"(\.L\w+):\n"
GCC_WORD_RE = r"(\w+) PTR"
GCC_FUNC_CALL_RE = r"(.+)@PLT\n"
GCC_RIP_RE = r"(\s(\w|\.)+)\[rip\]"

RENAME_LIST = [
    (".byte", "db"),
    ("short", "dw"),
    (".long", "dd"),
    (".quad", "dq"),
    (".zero", "resb"),
    (".string", "db"),
    (".value", "dd"),
    ("OFFSET FLAT:", ""),
    ("movabs", "mov"),
    ("rip", "rel"),
    ("\tcall\t__stack_chk_fail\n", ""),
    ('"', "`"),
]


def rename_symbols(name: str, text: str) -> str:
    prefix = name.replace("/", ".").replace(" ", ".")

    standard_symbols = re.findall(GCC_STANDARD_SYMBOL_RE, text)
    for symbol in standard_symbols:
        text = text.replace(symbol, f"{prefix}{symbol}")

    gcc_words = re.findall(GCC_WORD_RE, text)
    for symbol in gcc_words:
        text = text.replace(f"{symbol} PTR", symbol.lower())

    gcc_funcs = re.findall(GCC_FUNC_CALL_RE, text)
    for symbol in gcc_funcs:
        text = text.replace(f"{symbol}@PLT", symbol)

    gcc_rips = re.findall(GCC_RIP_RE, text)
    for rip in gcc_rips:
        rip = rip[0]
        text = text.replace(f"{rip}[rip]", f"\t[rel {rip}]")

    for original, new in RENAME_LIST:
        text = text.replace(original, new)

    return text


GCC_USELESS_START_RE = r"^((?:\t\.\w+.*\n)*)"
GCC_USELESS_END_RE = r"\t\.ident.*\n\t\.section.*$"
GCC_USELESS_SECTION_RE = r"(.*LFE(.|\n)+?\n)(((\w|\.)+:)|$)"
GCC_USELESS_SECTION_LINE_RE = r"\.section.*"
GCC_USELESS_INFO_RE = r"((?:\t\.text\n)?\t\.globl\t\w+\n(?:.|\n)+?\n)\w+:"
GCC_USELESS_LABEL_RE = r".*LFB\d+:\n"
GCC_USELESS_INSTRUCTIONS_RE = r"\t\.cfi.*\n"
GCC_COMMENTS_RE = r"#.*\n"


def relevant_asm(text: str) -> str:
    useless_starts = re.findall(GCC_USELESS_START_RE, text)
    for start in useless_starts:
        text = text.replace(start, "")

    useless_ends = re.findall(GCC_USELESS_END_RE, text)
    for end in useless_ends:
        text = text.replace(end, "")

    useless_sections = re.findall(GCC_USELESS_SECTION_RE, text)
    for section in useless_sections:
        text = text.replace(section[0], "")

    useless_sections = re.findall(GCC_USELESS_SECTION_LINE_RE, text)
    for section in useless_sections:
        text = text.replace(section, "")

    useless_info = re.findall(GCC_USELESS_INFO_RE, text)
    for info in useless_info:
        text = text.replace(info, "")

    useless_labels = re.findall(GCC_USELESS_LABEL_RE, text)
    for label in useless_labels:
        text = text.replace(label, "")

    comments = re.findall(GCC_COMMENTS_RE, text)
    for comment in comments:
        text = text.replace(comment, "")

    instructions = re.findall(GCC_USELESS_INSTRUCTIONS_RE, text)
    for instruction in instructions:
        text = text.replace(instruction, "")

    return text


def merge_asm(file_data: List[Tuple[str, str]], entrypoint: str) -> str:
    error = False

    symbols = {}
    for name, text in file_data:
        for symbol, type in get_symbols(text):
            if symbol == entrypoint and type != "function":
                print("Entrypoint must be a function!")
                error = True
            if symbol in symbols:
                symbols[symbol].append(name)
            else:
                symbols[symbol] = [name]

    for symbol, files in symbols.items():
        if len(files) > 1:
            print(
                f"Error: symbol {symbol} was found in multiple locations: {' '.join(files)}"
            )
            error = True

    if not entrypoint in symbols:
        print(f'Symbol for entrypoint "{entrypoint}" was not found')
        error = True

    if error:
        print("Exiting due to errors...")
        sys.exit(1)

    file_data = [(name, rename_symbols(name, text)) for name, text in file_data]

    merged_asm = ""
    for name, text in file_data:
        merged_asm += relevant_asm(text)
        merged_asm += "\n"

    merged_asm = merged_asm.replace("\t", " " * TAB_LENGTH)

    return merged_asm


def rename_entrypoint(template_elf_asm: str, entrypoint: str) -> str:
    return template_elf_asm.replace("entry_point", entrypoint)


ELF_ASM_ENDING = "file_end:"


def write_external_asm(template_elf_asm: str, external_asm: str, output_file: str):
    new_ending = f"{external_asm}\n{ELF_ASM_ENDING}"
    output_asm = template_elf_asm.replace(ELF_ASM_ENDING, new_ending)

    with open(output_file, mode="w") as o:
        o.write(output_asm)


def main():
    file_data, entrypoint, template_elf_asm, output_file = parse_args()
    merged_asm = merge_asm(file_data, entrypoint)
    template_elf_asm = rename_entrypoint(template_elf_asm, entrypoint)
    write_external_asm(template_elf_asm, merged_asm, output_file)


if __name__ == "__main__":
    main()
