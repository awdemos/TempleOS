# NeoTempleOS Compiler Makefile

CC      := gcc
CFLAGS  := -std=gnu99 -O0 -g -m64 -no-pie -fno-pie -w -Wno-int-conversion -Wno-incompatible-pointer-types
LDFLAGS := -lm -lrt

PROJECT := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
RUNTIME_DIR := $(PROJECT)/Neo/runtime
BUILD_DIR   := $(PROJECT)/Neo/build
PREPROC_DIR := $(BUILD_DIR)/preprocessed
COMPILER_SRC := $(PROJECT)/Compiler

INCLUDES := -I$(RUNTIME_DIR) -I$(BUILD_DIR) -I$(PREPROC_DIR)

TARGET := holyc_compiler

RUNTIME_CORE := $(RUNTIME_DIR)/holyc_core.c

HC_FILES := \
	Templates CExts CInit CExcept LexLib Lex \
	CHash UAsm PrsLib AsmInit AsmLib AsmResolve \
	Asm OptLib PrsExp PrsVar CMisc CMain PrsStmt \
	OptPass012 OptPass3 OptPass4 OptPass5 OptPass6 \
	BackLib BackFA BackFB BackA BackB BackC \
	OptPass789A

PREPROC_C := $(addprefix $(PREPROC_DIR)/, $(addsuffix .c, $(HC_FILES)))

.PHONY: all clean run rebuild

all: $(TARGET)

$(BUILD_DIR)/compiler.c: force_preprocess
	@true

force_preprocess:
	python3 $(BUILD_DIR)/build_compiler.py

$(TARGET): $(BUILD_DIR)/compiler.c $(RUNTIME_CORE)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(BUILD_DIR)/compiler.c $(RUNTIME_CORE) $(LDFLAGS)
	@echo "Built $(TARGET) successfully"

rebuild:
	$(MAKE) clean
	python3 $(BUILD_DIR)/build_compiler.py --rebuild
	$(MAKE)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(PREPROC_DIR)
	rm -f $(BUILD_DIR)/compiler.c
	rm -f $(TARGET)
	@echo "Clean done"
