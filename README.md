# QRception
A QR code generator that can generate itself

Created using [ISO standard](/iso_standard.pdf)

### Please leave issues, they can be about program functionality or improving the overall structure of the program

This QR code encodes the data of a QR code generating Linux x86-64 executable
![QR Code generator (all codes are max size)](/max_version_generator/qr_generator.bmp)


## Why
Why not?


## How to use

Pass in data directly - `./qr_generator "<data>"`

Pass in data through a file -`./qr_generator "" "<file_path>"`

Output is a bitmap file to stdout, pipe it where you see fit


## Goals
- Decently featured QR code generator that fits within a QR Code (<= 2953 bytes)
- **Features**
    - Chooses smallest mode
    - Supports all error correction levels (Low, Medium, Quality, High)
    - Supports numeric, alphanumeric, and byte encoding
    - Supports all versions from 1-40
    - Produces a complete QR code with no errors
    - Fits into QR Code (less than 2953 bytes)


## Project Status
There are two versions, the normal version is a typical QR code generator, the max version only produces version 40 (max size) QR codes

| Features | Normal | Max Version |
| --- | --- | --- |
| Chooses smallest version | ✅ | ❌(Only version 40) |
| Supports all versions | ✅ | ❌(Only version 40) |
| Supports all error correction levels | ❌(Only Low) | ❌(Only Low) |
| Supports all 3 encoding types | ✅ | ✅ |
| Produces QR Code w/ no errors | ✅ | ✅ |
| Fits into QR Code | ❌ (4821 bytes) | ✅ (2903 bytes) |


## Contributing - Help shrink either version, or add more features if space allows
If you are changing the source code, make sure the tests run. Even if all 40 versions work successfully, use `compare_qr_matrix.py` at least once and make sure it returns 0.

If you are changing tests, just make sure they run properly on the existing executable. Update requirements.txt if you are adding any python libraries.

If you are changing the build process, just make sure that you update `symbol_size.py` if anything regarding the object file it works with changes. No real reason to to do this unless some new code breaks the build process. 

**Warning** - The build process may break without any error messages. You will know this has happened if the behavior of `qr_generator` is different than `build/debug/qr_generator`.


## Overview
### Program
Not much to say here. One quirk is that `main.c` includes the other `.c` files directly since that led to a smaller binary size in my experience.


### Final executable
<details>
<summary>Old methods for creating final binary</summary>

I have always used `elf_template.asm` as a starter and have only modified it slightly.

#### Source code -> MASM -> NASM
I used gcc to compile the source code into separate asm files. However, these could not be NASM, so I chose the closest alternative, MASM, and wrote a jank python script to attempt to convert between the two using the errors that I found, then append to the template.
Eventually, the program was crashing and faulting in different ways and I realized this was not a reliable strategy.


#### Source code -> Object file -> NASM
I used gcc to compile the source code to an object file, then used [`objconv`](https://github.com/gitGNU/objconv) to disassemble the object file into mostly valid NASM. Then I had a python script clean it up and append it to the template.
    
This also just stopped working eventually.
</details>

1. Assembles `util/elf_template.asm` into a binary file
2. Compiles `main.c` into an object file, excluding builtins, startfiles, and the stdlib
3. Uses `objcopy` to only extract `.text.startup` (main), `.text` (all other functions), and `.rodata` (all constants) and sets their ALIGN to 1 so there are no wasted bytes
4. Links the filtered object file with `util/linker.ld` such that `main` is at the very start. Formats it as a binary, so the data is just directly put into the file, only address resolution occurs
5. Corrects the elf headers from the template to have the correctly calculated file size
6. Appends the binary file from linking to the elf headers to create an executable. Because of relative addressing all the addresses are still correct.

## General repo structure
### Repo root

`build/` - Stores build artifacts

`max_version_generator/` - Stores code for generator that only supports max version (version 40)

`src/` - C source code

`tests/` - Tests for both final QR validity and some intermediate steps

`util/` - Stores utility files that help in the build

`qr_generator` - Final program

`Makefile` - Handles building both debug and final executables


### Tests overview
[segno](https://github.com/heuer/segno) is a python QR code generation library that is used as a validator for tests

[zbarimg](https://github.com/mchehab/zbar) is a QR code scanning tool that is used as a scanner for tests

[pillow](https://github.com/python-pillow/Pillow) is a python library for imaging and was used to parse bitmap files


`test.py` - Tests program with QR data of byte and alphanumeric data for all versions

`compare_qr_matrix.py` - Compares the QR Matrix generated between program and segno

`compare_final_message.py` - Run this with gdb. Compares the final QR message (data and error correction) between program and segno

`compare_data_codewords.py` - Run this with gdb. Compares the the data codewords between program and segno


### Util overview
[Tiny ELF Files](https://nathanotterness.com/2021/10/tiny_elf_modernized.html) is to credit for `elf_template.asm`

`elf_template.asm` - Template NASM file that is used as the start of the final elf64 executable

`linker.ld` - Linker that extracts only the necessary parts out of the object file compiled from the source code

`symbol_size.py` - Program that calculates and prints out the sizes of the different symbols in the source code for analysis
