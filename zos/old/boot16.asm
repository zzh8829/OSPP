[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x9c00
	jmp main16

main16:

	xor ax, ax
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov bp, 0x9c00		; set stack position
	mov sp, bp

	mov [BOOT_DRIVE], dl 	; boto drive

	mov bx, KERNEL_OFFSET
	mov dh, 2
	mov dl, [BOOT_DRIVE]
	call loadDisk

	mov ax, KERNEL_OFFSET
	call printSector

	jmp $

%include "print16.asm"
%include "disk.asm"


times 510-($-$$) db 0
dw 0xaa55

times 0x50 db 0x11
times 0x50 db 0x22
times 0x50 db 0x33
times 0x50 db 0x44
times 0x50 db 0x55
times 0x50 db 0x66

times 1024-($-$$) db 0
