.DEFAULT_GOAL := all

CC := gcc
CFLAGS := -Wall -Wextra -masm=intel -Os

SRCDIR := src
BUILDDIR := build

BUILD_SCRIPT := merge_asm.py
ELF_TEMPLATE := elf_template.asm
ENTRYPOINT := start

SRCS := $(shell find $(SRCDIR) -name '*.c')

SRCASMS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.s, $(SRCS))

TARGET := qr_generator
FULL_ELF_ASM := $(BUILDDIR)/$(TARGET).asm

$(TARGET) : $(SRCASMS)
	python3 $(BUILD_SCRIPT) $(SRCASMS)\
		--entrypoint $(ENTRYPOINT)\
		--elf-template $(ELF_TEMPLATE)\
		--output $(FULL_ELF_ASM)
	nasm -f bin -o $(TARGET) $(FULL_ELF_ASM)
	chmod +x $(TARGET)


$(BUILDDIR)/%.s : $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -S $< -o $@

all: $(TARGET)

clean:
	rm -rf build/
	rm $(TARGET)
	@echo "Cleaned build artifacts"
