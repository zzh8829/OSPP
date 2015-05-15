#include "cpu.h"


uint8_t read_CMOS(uint8_t addr)
{
	uint8_t ret;
	cli();
	outb(0x70,addr);
	ret = inb(0x71);
	sti();
	return ret;
}

void write_CMOS(uint8_t addr, uint8_t value)
{
	cli();
	outb(0x70, addr);
	outb(0x71, value);
//	__asm__ volatile ("jmp 1f; 1: jmp 1f;1:");
	sti();
}



void reboot()
{
#define KB_KDATA 0x00000001
#define KB_UDATA 0x00000002
	cli();
	uint8_t t;
	do {
		t = inb(0x64);
		if( t & KB_KDATA )
			inb(0x60);
	} while( t & KB_UDATA );
	outb(0x64, 0xFE); /* send reboot command */
	hlt();
}

void sti()
{
	__asm__ volatile ("sti");
}

void cli()
{
	__asm__ volatile ("cli");
}

void hlt()
{
	__asm__ volatile ("hlt");
}