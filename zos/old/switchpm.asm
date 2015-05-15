[bits 16]
swithProtectedMode:

enable_a20:
	in al, 0x64
	test al, 0x2
	jnz enable_a20
	mov al, 0xdf
	out 0x64, al
	
	cli 
	lgdt [GDT_descriptor]

	mov eax, cr0
	or eax, 0x1
	mov cr0, eax

	jmp CODE_SEG: initProtectedMode

[bits 32]
initProtectedMode:
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov ebp, 0x90000		; stack position
	mov esp, ebp

	call main32				; protected mode main
