.DEFAULT_GOAL := all

CC := gcc
CFLAGS := -Wall -Wextra -masm=intel -Oz -fno-builtin -nostdlib -nostartfiles -fno-stack-protector
DBG_CFLAGS := -Wall -Wextra -O0 -g

SRCDIR := src
BUILDDIR := build
DBGDIR := $(BUILDDIR)/debug

BUILD_SCRIPT := merge_asm.py
ELF_TEMPLATE := elf_template.asm
ENTRYPOINT := main

WATCHED_FILES := $(shell find $(SRCDIR) -name '*.c')
SRCS := $(SRCDIR)/main.c

SRCASMS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.s, $(SRCS))
DBGOBJS := $(patsubst $(SRCDIR)/%.c, $(DBGDIR)/%.o, $(SRCS))

TARGET := qr_generator
DBG_TARGET := $(DBGDIR)/$(TARGET)
TARGET_OBJ := $(BUILDDIR)/$(TARGET).o
PREPROCESSED_TARGET := $(BUILDDIR)/qr_generator.E
FULL_ELF_ASM := $(BUILDDIR)/$(TARGET).asm

$(FULL_ELF_ASM) : $(SRCASMS)
	python3 $(BUILD_SCRIPT) $(SRCASMS)\
		--entrypoint $(ENTRYPOINT)\
		--elf-template $(ELF_TEMPLATE)\
		--output $(FULL_ELF_ASM)

$(TARGET) : $(FULL_ELF_ASM)
	nasm -f bin -o $(TARGET) $(FULL_ELF_ASM)
	chmod +x $(TARGET)

$(PREPROCESSED_TARGET) : $(SRCS) $(WATCHED_FILES)
	@mkdir -p $(BUILDDIR)
	$(CC) -E $< -o $@

$(TARGET_OBJ) : $(FULL_ELF_ASM)
	nasm -f elf64 -o $(TARGET_OBJ) $(FULL_ELF_ASM)

$(BUILDDIR)/%.s : $(SRCDIR)/%.c $(WATCHED_FILES)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -S $< -o $@

$(DBG_TARGET) : $(DBGOBJS)
	@mkdir -p $(DBGDIR)
	$(CC) $(DBGOBJS) -o $@

$(DBGDIR)/%.o : $(SRCDIR)/%.c $(WATCHED_FILES)
	@mkdir -p $(DBGDIR)
	$(CC) $(DBG_CFLAGS) -c $< -o $@

all: debug final symbol_sizes size

asm: $(FULL_ELF_ASM)

expand: $(PREPROCESSED_TARGET)

debug: $(DBG_TARGET)

final: $(TARGET)

symbol_sizes: $(TARGET_OBJ) $(TARGET)
	@python3 symbol_size.py "$$(nm -n $(TARGET_OBJ))" --file-size $$(stat -c %s $(TARGET))

size: $(TARGET)
	@echo "Binary has size $$(stat -c %s $(TARGET)) bytes"

clean:
	rm -rf build/
	rm $(TARGET)
	@echo "Cleaned build artifacts"
