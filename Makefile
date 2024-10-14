.DEFAULT_GOAL := all

CC := gcc
CFLAGS := -Wall -Wextra -masm=intel -Oz -fno-builtin -nostdlib -nostartfiles -fno-stack-protector
DBG_CFLAGS := -Wall -Wextra -Og -g

SRCDIR := src
BUILDDIR := build
DBGDIR := $(BUILDDIR)/debug

BUILD_SCRIPT := merge_asm.py
ELF_TEMPLATE := elf_template.asm
ENTRYPOINT := main

SRCS := $(shell find $(SRCDIR) -name '*.c')

SRCASMS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.s, $(SRCS))
DBGOBJS := $(patsubst $(SRCDIR)/%.c, $(DBGDIR)/%.o, $(SRCS))

TARGET := qr_generator
DBG_TARGET := $(DBGDIR)/$(TARGET)
FULL_ELF_ASM := $(BUILDDIR)/$(TARGET).asm

$(TARGET) : $(SRCASMS)
	python3 $(BUILD_SCRIPT) $(SRCASMS)\
		--entrypoint $(ENTRYPOINT)\
		--elf-template $(ELF_TEMPLATE)\
		--output $(FULL_ELF_ASM)
	nasm -f bin -o $(TARGET) $(FULL_ELF_ASM)
	chmod +x $(TARGET)
	@echo "Binary has size $$(stat -c %s $(TARGET)) bytes"

$(BUILDDIR)/%.s : $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -S $< -o $@

$(DBG_TARGET) : $(DBGOBJS)
	@mkdir -p $(DBGDIR)
	$(CC) $(DBGOBJS) -o $@

$(DBGDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(DBGDIR)
	$(CC) $(DBG_CFLAGS) -c $< -o $@

all: debug final

debug: $(DBG_TARGET)

final: $(TARGET)

clean:
	rm -rf build/
	rm $(TARGET)
	@echo "Cleaned build artifacts"
