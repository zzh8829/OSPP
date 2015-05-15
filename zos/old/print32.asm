[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_BLACK equ 0x0f

printString32:
	pusha
	mov eax,0
	mov ax, [ypos]
	mov dx, 80
	mul dx
	add ax, [xpos]
	shl ax, 1
	mov edx, VIDEO_MEMORY
	add edx, eax
	
	.ps32_next:
		lodsb
		cmp al,0
		je .ps32_end
		mov ah, 0x0f
		mov [edx], ax
		add edx, 2
		add byte [xpos], 1
		jmp .ps32_next
		
	.ps32_end:
		add byte [ypos], 1
		mov byte [xpos], 0

	call set_cursor
	popa
	ret

set_cursor:
	
	mov ax, [ypos]
	mov dx, 80
	mul dx
	add ax, [xpos]
	mov bx,ax

	mov dx, 0x3d4
	mov al, 14
	out dx,al
	mov dx, 0x3d5
	mov al, bh
	out dx,al

	mov dx, 0x3d4
	mov al, 15
	out dx,al
	mov dx, 0x3d5
	mov al,bl
	out dx,al
	ret

ypos db 0
xpos db 0