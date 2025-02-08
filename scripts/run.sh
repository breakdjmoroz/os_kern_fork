#!/bin/bash
set -xue

QEMU=qemu-system-riscv32

KERN=$1
DISK=$2

(cd disk && tar cf ../disk.tar --format=ustar *)

$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -d unimp,guest_errors,int,cpu_reset -D qemu.log \
    -drive id=drive0,file=$DISK,format=raw,if=none \
    -device virtio-blk-device,drive=drive0,bus=virtio-mmio-bus.0 \
    -kernel $KERN
