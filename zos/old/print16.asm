[bits 16]
; print char from al
printChar:				
	mov ah, 0x0e		; enable print mode
	int 0x10			; print interrupt
	ret

; print new line
printEndl:				
	mov al, 0
	stosb
	mov al, 0x0d		; vertical tab
	call printChar
	mov al, 0x0a		; new line
	call printChar
	ret

; print string from si
printString16:
	.next:
		lodsb			; Load String
		or al, al 		; if reach end
		jz .exit		; exit
		call printChar	; print char
		jmp .next		; continue loop
	.exit:
		call printEndl
	ret

; print hex from ax
printHex:
	push ax				; save ax
	mov al,ah 			; high half
	call printByte
	pop ax				; restore ax
	call printByte		; low half
	ret

; print byte from al
printByte:
	push ax				; save ax
	shr al,0x4			; hight half
	call printNibble
	pop ax				; restore ax
	and al,0xf			; low half
	call printNibble
	ret

; print nibble from al
printNibble:
	cmp al,0x0a 		; if greater than 10 (0xa)
	jge .alpha			; jump to print alpha
	add al,'0'			; print digit
	call printChar
	ret					; return 
	.alpha:
		add al,'A'		; +"A"- 0xa
		sub al,0xa
		call printChar
	ret 				; return
