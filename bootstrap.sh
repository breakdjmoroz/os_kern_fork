#!/bin/bash

./build_mini.sh #build mini_kernel from source

xxd -p mini_kernel.elf > mini_kernel.img  #dump mini_kernel to hex

./run_mini.sh mini_kernel.elf mini_kernel.img #self-build mini_kernel

xxd -p mini_kernel.img > self_mini_kernel.img  #dump self_mini_kernel to hex

./run_mini.sh mini_kernel.img self_mini_kernel.img #self-self-build mini_kernel

./build.sh #build kernel from source 

xxd -p kernel.elf > kernel.img #dump kernel

./run.sh mini_kernel.img kernel.img #self-build kernel

./run.sh kernel.img disk.tar #test kernel
