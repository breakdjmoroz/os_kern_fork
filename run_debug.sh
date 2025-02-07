#!/bin/bash
set -xue

QEMU=qemu-system-riscv32
CC=clang
OBJCOPY=llvm-objcopy

CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"

KERN=mini_kernel.img
DISK=ltest_hd0

# Build the shell.
$CC $CFLAGS -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.c user.c common.c
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

# Build the kernel.
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o mini_kernel.elf \
    mini_kernel.c shell.bin.o

(cd disk && tar cf ../disk.tar --format=ustar *.txt)
cp test test_hd0
cp ltest_tmp ltest_hd0
cp permut4x permut_hd0

$QEMU -s -S -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -d unimp,guest_errors,int,cpu_reset -D qemu.log \
    -drive id=drive0,file=$DISK,format=raw,if=none \
    -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
    -kernel $KERN
