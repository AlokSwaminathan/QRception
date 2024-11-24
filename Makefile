.DEFAULT_GOAL := all

CC := gcc
CFLAGS := -Wall -Wextra -Oz -fno-builtin -nostdlib -nostartfiles -fno-stack-protector
DBG_CFLAGS := -Wall -Wextra -O0 -g

OBJCOPY := objcopy
OBJCOPY_FLAGS := --only-section=.text --only-section=.text.startup --only-section=.rodata --set-section-alignment .rodata=1

NASM := nasm
NASM_FLAGS := -f bin

LINKER := ld
LINKER_FLAGS := --oformat=binary

SRCDIR := src
BUILDDIR := build
DBGDIR := $(BUILDDIR)/debug

WATCHED_FILES := $(shell find $(SRCDIR) -name '*.c')
SRC := $(SRCDIR)/main.c

DBGOBJS := $(patsubst $(SRCDIR)/%.c, $(DBGDIR)/%.o, $(SRC))

TARGET := qr_generator
DBG_TARGET := $(DBGDIR)/$(TARGET)
INTERMEDIATE_OBJ := $(BUILDDIR)/$(TARGET)_full.o
TARGET_OBJ := $(BUILDDIR)/$(TARGET).o
TARGET_BIN := $(BUILDDIR)/qr_generator.bin
PREPROCESSED_TARGET := $(BUILDDIR)/qr_generator.E

UTIL_DIR = util

SYMBOL_SIZE := $(UTIL_DIR)/symbol_size.py
ELF_TEMPLATE := $(UTIL_DIR)/elf_template.asm
ELF_STUB := $(BUILDDIR)/elf_stub
# Created with the wrong segment size so the size of the stub itself can be measured
ELF_STUB_NAIVE := $(BUILDDIR)/elf_stub_naive
LINKER_SCRIPT := $(UTIL_DIR)/linker.ld

PURE_FILE_SIZE_CMD := stat -c %s $(TARGET)

COMBINED_FILE_SIZE_CMD := wc -c $(ELF_STUB_NAIVE) $(TARGET_BIN) | tail -n 1 | awk '{print $$1}'

$(TARGET) : $(TARGET_BIN) $(ELF_STUB)
	cat $(ELF_STUB) $(TARGET_BIN) > $(TARGET)
	chmod +x $(TARGET)

$(PREPROCESSED_TARGET) : $(SRCS) $(WATCHED_FILES)
	@mkdir -p $(BUILDDIR)
	$(CC) -E $< -o $@

$(INTERMEDIATE_OBJ) : $(SRC) $(WATCHED_FILES)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -s -c $< -o $@

$(TARGET_OBJ) : $(INTERMEDIATE_OBJ)
	$(OBJCOPY) $(OBJCOPY_FLAGS) $< $@	

$(TARGET_BIN) : $(TARGET_OBJ)
	$(LINKER) $(LINKER_FLAGS) -T $(LINKER_SCRIPT) $< -o $@

$(ELF_STUB) : $(ELF_STUB_NAIVE) $(TARGET_BIN)
	$(NASM) $(NASM_FLAGS) -D FILE_SIZE=$$($(COMBINED_FILE_SIZE_CMD)) $(ELF_TEMPLATE) -o $@

$(ELF_STUB_NAIVE) : $(ELF_TEMPLATE)
	$(NASM) $(NASM_FLAGS) $(ELF_TEMPLATE) -o $@

$(DBG_TARGET) : $(DBGOBJS)
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
	@python3 $(SYMBOL_SIZE) $(TARGET_OBJ)

size: $(TARGET)
	@echo "Binary has size $$($(PURE_FILE_SIZE_CMD)) bytes"

clean:
	rm -rf build/
	rm $(TARGET)
	@echo "Cleaned build artifacts"
