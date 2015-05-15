#include "isrs.h"
#include "kstdio.h"

void _isr0 ();
void _isr1 ();
void _isr2 ();
void _isr3 ();
void _isr4 ();
void _isr5 ();
void _isr6 ();
void _isr7 ();
void _isr8 ();
void _isr9 ();
void _isr10();
void _isr11();
void _isr12();
void _isr13();
void _isr14();
void _isr15();
void _isr16();
void _isr17();
void _isr18();
void _isr19();
void _isr20();
void _isr21();
void _isr22();
void _isr23();
void _isr24();
void _isr25();
void _isr26();
void _isr27();
void _isr28();
void _isr29();
void _isr30();
void _isr31();
void _isr127();

isrs_handler_t isrs_routines[256] = { NULL };

void isrs_install_handler(size_t isrs, isrs_handler_t handler) {
	isrs_routines[isrs] = handler;
}

void isrs_uninstall_handler(size_t isrs) {
	isrs_routines[isrs] = 0;
}

void isrs_init() {
	/* Exception Handlers */
	memset(isrs_routines, 0x00, sizeof(isrs_routines));
	idt_set(0, (uint32_t)_isr0,  0x08, 0x8E);
	idt_set(1, (uint32_t)_isr1,  0x08, 0x8E);
	idt_set(2, (uint32_t)_isr2,  0x08, 0x8E);
	idt_set(3, (uint32_t)_isr3,  0x08, 0x8E);
	idt_set(4, (uint32_t)_isr4,  0x08, 0x8E);
	idt_set(5, (uint32_t)_isr5,  0x08, 0x8E);
	idt_set(6, (uint32_t)_isr6,  0x08, 0x8E);
	idt_set(7, (uint32_t)_isr7,  0x08, 0x8E);
	idt_set(8, (uint32_t)_isr8,  0x08, 0x8E);
	idt_set(9, (uint32_t)_isr9,  0x08, 0x8E);
	idt_set(10, (uint32_t)_isr10, 0x08, 0x8E);
	idt_set(11, (uint32_t)_isr11, 0x08, 0x8E);
	idt_set(12, (uint32_t)_isr12, 0x08, 0x8E);
	idt_set(13, (uint32_t)_isr13, 0x08, 0x8E);
	idt_set(14, (uint32_t)_isr14, 0x08, 0x8E);
	idt_set(15, (uint32_t)_isr15, 0x08, 0x8E);
	idt_set(16, (uint32_t)_isr16, 0x08, 0x8E);
	idt_set(17, (uint32_t)_isr17, 0x08, 0x8E);
	idt_set(18, (uint32_t)_isr18, 0x08, 0x8E);
	idt_set(19, (uint32_t)_isr19, 0x08, 0x8E);
	idt_set(20, (uint32_t)_isr20, 0x08, 0x8E);
	idt_set(21, (uint32_t)_isr21, 0x08, 0x8E);
	idt_set(22, (uint32_t)_isr22, 0x08, 0x8E);
	idt_set(23, (uint32_t)_isr23, 0x08, 0x8E);
	idt_set(24, (uint32_t)_isr24, 0x08, 0x8E);
	idt_set(25, (uint32_t)_isr25, 0x08, 0x8E);
	idt_set(26, (uint32_t)_isr26, 0x08, 0x8E);
	idt_set(27, (uint32_t)_isr27, 0x08, 0x8E);
	idt_set(28, (uint32_t)_isr28, 0x08, 0x8E);
	idt_set(29, (uint32_t)_isr29, 0x08, 0x8E);
	idt_set(30, (uint32_t)_isr30, 0x08, 0x8E);
	idt_set(31, (uint32_t)_isr31, 0x08, 0x8E);
	idt_set(0x7F, (uint32_t)_isr127, 0x08, 0x8E);
}

static const char *exception_messages[32] = {
	"Division by zero",				/* 0 */
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",				/* 5 */
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",						/* 10 */
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",			/* 15 */
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void dump_info(struct regs *r)
{
	kprintf(PANIC,"Data Dump:\n");
	kprintf(PANIC,"gs : %x, fs : %x, es : %x, ds : %x\n", r->gs, r->fs, r->es, r->ds);
	kprintf(PANIC,"edi: %x, esi: %x, ebp: %x, esp: %x\n", r->edi, r->esi, r->ebp, r->esp);
	kprintf(PANIC,"ebx: %x, edx: %x, ecx: %x, eax: %x\n", r->ebx, r->edx, r->ecx, r->eax);
	kprintf(PANIC,"int_no: %x, err_code: %x\n", r->int_no, r->err_code);
	kprintf(PANIC,"eip: %x, cs : %x, eflags: %x\n", r->eip, r->cs, r->eflags);
	kprintf(PANIC,"useresp: %x, ss: %x\n", r->useresp, r->ss);
}

void fault_handler(struct regs *r) 
{
	dump_info(r);
	isrs_handler_t handler = isrs_routines[r->int_no];
	if (handler) 
	{
		handler(r);
	} else {
		kprintf(PANIC,"Unhandled Exception: [%d] %s\n", r->int_no, exception_messages[r->int_no]);
	}
	for(;;);
}