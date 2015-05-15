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

	mov [BOOT_DRIVE], dl

	mov si, MSG_WELCOME
	call printString16

	call printEndl

	mov si, MSG_LOADKERNEL
	call printString16

	call loadKernel

	call swithProtectedMode

	jmp $

%include "print16.asm"
%include "print32.asm"
%include "gdt.asm"
%include "switchpm.asm"
%include "disk.asm"

[bits 16]

loadKernel:

	mov bx, KERNEL_OFFSET
	mov dh, 5
	mov dl, [BOOT_DRIVE]
	;call loadDisk

	ret

[bits 32]

main32:

	add byte [ypos], 3

	mov esi, MSG_PMODE
	call printString32

	;call KERNEL_OFFSET

	jmp $

MSG_WELCOME 	db "Welcome to Zihao OS",0
MSG_LOADKERNEL 	db "loading Kernel...",0
MSG_PMODE		db "[ok]",0

times 510-($-$$) db 0
dw 0xaa55
