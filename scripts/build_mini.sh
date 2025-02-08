#!/bin/bash
set -xue

CC=clang
OBJCOPY=llvm-objcopy

CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"

# Build the mini_kernel.
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o mini_kernel.elf \
    mini_kernel.c shell.bin.o

