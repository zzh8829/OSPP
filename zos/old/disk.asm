; load dh sectors to es:bx from drive dl
loadDisk:
	push dx

	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov dh, 0x00
	mov cl, 0x2

	int 0x13

	jc loadDiskError

	pop dx

	cmp dh,al
	jne loadDiskError

	ret

; Error
loadDiskError:
	mov si, MSG_DISK_LOAD_ERROR
	call printString16
	jmp $


MSG_DISK_LOAD_ERROR db "Read disk Err",0

BOOT_DRIVE db 0