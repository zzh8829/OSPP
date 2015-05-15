# Programs
AS = i586-elf-as
LD = i586-elf-ld 
CC = i586-elf-gcc

# Directories
INC_DIR = include
BUILD_DIR = build
SRC_DIR = src
BIN_DIR = bin


# Flags
INCFLAG 	= -I${INC_DIR}
STDFLAG 	= -std=gnu11
ARCHFLAG 	= -march=pentium4
OPTFLAG 	= -O2
WARNING 	= -Wall -Wextra
NOWARNING 	= -Wno-unused-function -Wno-unused-parameter -Wno-sign-compare \
				-Wno-trigraphs -Wno-unused-variable -Wno-pointer-sign \
				-Wno-unused-but-set-variable

CFLAGS 		=  ${ARCHFLAG} ${OPTFLAG} ${INCFLAG} ${STDFLAG} \
				${WARNING} ${NOWARNING} -ffreestanding 

# Sources Objects
KERNEL_SRCS = $(wildcard src/*.c)
KERNEL_OBJS = $(subst ${SRC_DIR}/,${BUILD_DIR}/,${KERNEL_SRCS:.c=.o})

BOOT_SRC = ${SRC_DIR}/boot.s
BOOT_OBJ = ${BUILD_DIR}/boot.o

# Link Script
LINKSCRIPT = src/link.ld

# Build

all: build_dir zos_kernel

build_dir:
	mkdir -p ${BUILD_DIR}
	mkdir -p ${BIN_DIR}

# Kernel

zos_kernel: ${LINKSCRIPT} ${BOOT_OBJ} ${KERNEL_OBJS}
	${LD} -T ${LINKSCRIPT} -o $@ ${BOOT_OBJ} ${KERNEL_OBJS}

${BOOT_OBJ}: ${BOOT_SRC}
	${AS} ${INCFLAG} $< -o $@

${BUILD_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} ${CFLAGS} -c -o $@ $<

# Clean

clean:
	find -iname "*.o" -exec rm {} \;
	rm -f zos.iso
	rm -f zos_kernel

# Arguments

iso: zos_kernel grub.cfg
	cp grub.cfg isodir/boot/grub/grub.cfg
	cp zos_kernel isodir/boot/zos_kernel
	grub-mkrescue -o zos.iso isodir

usb: zos_kernel
	cp grub.cfg /media/zihao/zos/boot/grub/grub.cfg
	cp zos_kernel /media/zihao/zos/boot/zos_kernel

qemu: iso
	sudo qemu -cdrom zos.iso \
	-boot order=d \
	-drive if=none,id=usbstick,file=/dev/sdc \
	-usb \
	-device usb-ehci,id=ehci \
	-device usb-storage,bus=ehci.0,drive=usbstick

vb: iso
	cd script; \
	python3 runvb.py

run: iso
	bochs -f script/bochs.rc