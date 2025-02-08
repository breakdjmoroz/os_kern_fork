CC=clang
OBJCOPY=llvm-objcopy

CFLAGS=-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib

ROOT=$(shell pwd)
BUILD=$(ROOT)/build
IMG=$(ROOT)/img
SRC=$(ROOT)/src
SCRIPTS=$(ROOT)/scripts

all: init build_full build_mini
		xxd -p $(BUILD)/mini_kernel.elf > $(IMG)/mini_kernel.img                    # dump mini_kernel to hex
		$(SCRIPTS)/run_mini.sh $(BUILD)/mini_kernel.elf $(IMG)/mini_kernel.img      # self-build mini_kernel by source-build one
		xxd -p $(IMG)/mini_kernel.img > $(IMG)/self_mini_kernel.img                 # dump self_mini_kernel to hex
		$(SCRIPTS)/run_mini.sh $(IMG)/mini_kernel.img $(IMG)/self_mini_kernel.img   # self-build mini_kernel by self-build one
		xxd -p $(BUILD)/kernel.elf > $(IMG)/kernel.img                              # dump kernel to hex
		$(SCRIPTS)/run.sh $(IMG)/self_mini_kernel.img $(IMG)/kernel.img             # self-build kernel
		$(SCRIPTS)/run.sh $(IMG)/kernel.img $(ROOT)/disk.tar                        # test kernel

# Build the mini_kernel.
build_mini:
		echo "Start building mini_kernel.elf..."
		$(CC) $(CFLAGS) -Wl,-T$(SRC)/kernel.ld -Wl,-Map=$(SRC)kernel.map -o $(BUILD)/mini_kernel.elf $(SRC)/mini_kernel.c
		echo "Building mini_kernel.elf is finished successfuly!"

# Build the kernel.
build_full:
		ROOT=$(ROOT) make -C $(SRC)
		
init:
		mkdir -p $(BUILD) $(IMG)

clear:
		rm $(IMG)/* $(BUILD)/*
		echo "All is clear!"
