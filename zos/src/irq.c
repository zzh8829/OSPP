#include "irq.h"
#include "kstdio.h"

void _irq0 ();
void _irq1 ();
void _irq2 ();
void _irq3 ();
void _irq4 ();
void _irq5 ();
void _irq6 ();
void _irq7 ();
void _irq8 ();
void _irq9 ();
void _irq10();
void _irq11();
void _irq12();
void _irq13();
void _irq14();
void _irq15();

void irq_remap()
{
	outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void irq_install_handler(size_t irq, irq_handler_t handler) {
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(size_t irq) {
	irq_routines[irq] = 0;
}

void irq_init()
{
	irq_remap();
	memset(irq_routines, 0, sizeof(irq_routines));
	idt_set(32,(uint32_t)_irq0,0x08,0x8E);
	idt_set(33,(uint32_t)_irq1,0x08,0x8E);
	idt_set(34,(uint32_t)_irq2,0x08,0x8E);
	idt_set(35,(uint32_t)_irq3,0x08,0x8E);
	idt_set(36,(uint32_t)_irq4,0x08,0x8E);
	idt_set(37,(uint32_t)_irq5,0x08,0x8E);
	idt_set(38,(uint32_t)_irq6,0x08,0x8E);
	idt_set(39,(uint32_t)_irq7,0x08,0x8E);
	idt_set(40,(uint32_t)_irq8,0x08,0x8E);
	idt_set(41,(uint32_t)_irq9,0x08,0x8E);
	idt_set(42,(uint32_t)_irq10,0x08,0x8E);
	idt_set(43,(uint32_t)_irq11,0x08,0x8E);
	idt_set(44,(uint32_t)_irq12,0x08,0x8E);
	idt_set(45,(uint32_t)_irq13,0x08,0x8E);
	idt_set(46,(uint32_t)_irq14,0x08,0x8E);
	idt_set(47,(uint32_t)_irq15,0x08,0x8E);
}

void irq_handler(struct regs *r)
{
	if(r->int_no >= 40)
	{
		outb(0xA0,0x20);
	} else if(r->int_no >= 32) {
		irq_handler_t handler = irq_routines[r->int_no-32];
		if(handler)
		{
			handler(r);
		}
	}
	outb(0x20,0x20);
}