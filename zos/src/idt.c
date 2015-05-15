#include "idt.h"

void idt_set(uint8_t num, uint32_t base, uint16_t sel,uint8_t flags)
{
	idt[num].base_low =		((uint32_t)base & 0xFFFF);
	idt[num].base_high =	((uint32_t)base >> 16) & 0xFFFF;
	idt[num].sel =			sel;
	idt[num].zero =			0;
	idt[num].flags =		flags | 0x60;
}

void idt_init()
{
	/* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);


    /* Points the processor's internal register to the new IDT */
    idt_load();
    __asm__("sti"::);
}