#!/bin/sh

nasm -o boot.bin -f bin boot.asm
nasm kernel_entry.asm -f elf -o kernel_entry.o
i586-elf-gcc -ffreestanding -c vbe.c -o vbe.o
i586-elf-gcc -ffreestanding -c kernel.c -o kernel.o
i586-elf-ld -o kernel.bin -Ttext 0x9c00 kernel_entry.o kernel.o --oformat binary

cat boot.bin kernel.bin > boot.img

./run.py