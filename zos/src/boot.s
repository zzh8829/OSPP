#define ASM_FILE 1

.set GFXOPT, 0

.set ALIGN,	1<<0			
.set MEMINFO,  1<<1
.set USEGFX,   1<<2			
.set FLAGS,	ALIGN | MEMINFO | USEGFX
.set MAGIC,	0x1BADB002	  
.set CHECKSUM, -(MAGIC + FLAGS) 
 
.section .multiboot
.align 4
#multiboot header
.long MAGIC
.long FLAGS
.long CHECKSUM
#elf section
.long 0x00000000 # header_addr
.long 0x00000000 # load_addr
.long 0x00000000 # load_end_addr
.long 0x00000000 # bss_end_addr
.long 0x00000000 # entry_addr
# graphics requests
.long GFXOPT # 0 = linear graphics
.long 1366
.long 768
.long 32		# Set me to 32 or else.

.section .text
.global _start
_start:

	movl $stack_top, %esp

	pushl %eax # header magic
	pushl %ebx # header pointer
	cli
	call kernel_main

	hlt
.hang:
	jmp .hang

.global gdt_load
.extern gp
gdt_load:
	lgdt gp		# Load the GDT with our '_gp' which is a special pointer
	mov $0x10, %ax	 # 0x10 is the offset in the GDT to our data segment
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x08,$gdt_ret   # 0x08 is the offset to our code segment: Far jump!
gdt_ret:
	ret   
	
.global idt_load
.extern idtp
idt_load:
	lidt idtp
	ret

.macro ISR_NO_ERR no 
.global _isr\no
_isr\no:
	pushl $0
	pushl $\no
	jmp isr_common_stub
.endm

.macro ISR_ERR no 
.global _isr\no
_isr\no: 
	pushl $\no
	jmp isr_common_stub
.endm

ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR   8
ISR_NO_ERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_NO_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31
ISR_NO_ERR 127

.extern fault_handler
isr_common_stub:		
	pushal	
	pushl   %ds
	pushl   %es
	pushl   %fs
	pushl   %gs
	movw	$0x10,%ax	# Load the Kernel Data Segment descriptor!
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movl	%esp,%eax	# Push us the stack
	pushl	%eax
	movl 	$fault_handler, %eax
	call	*%eax
	popl	%eax
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal	
	addl	$8,%esp	# Cleans up the pushed error code and pushed ISR number
	sti
	iret		# pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

.macro IRQ no, no2
.global _irq\no
_irq\no:
	cli
	pushl $0
	pushl $\no2
	jmp irq_common_stub
.endm

IRQ 0,32
IRQ 1,33
IRQ 2,34
IRQ 3,35
IRQ 4,36
IRQ 5,37
IRQ 6,38
IRQ 7,39
IRQ 8,40
IRQ 9,41
IRQ 10,42
IRQ 11,43
IRQ 12,44
IRQ 13,45
IRQ 14,46
IRQ 15,47

.extern irq_handler
irq_common_stub:
	pushal	
	pushl   %ds
	pushl   %es
	pushl   %fs
	pushl   %gs
	movw	$0x10,%ax	# Load the Kernel Data Segment descriptor!
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movl	%esp,%eax	# Push us the stack
	pushl	%eax
	movl 	$irq_handler, %eax
	call	*%eax
	popl	%eax
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	popal	
	addl	$8,%esp	# Cleans up the pushed error code and pushed ISR number
	sti
	iret		# pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

.section .bss
stack_bottom:
.lcomm bootstrap_stack 8192 # 16 KiB
stack_top:
 