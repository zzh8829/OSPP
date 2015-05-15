#include "timer.h"
#include "irq.h"
#include "kstdio.h"

uint32_t TICKS_PER_SEC = 1000;
volatile uint32_t timer_ticks;

void timer_phase(int hz)
{
	int divisor = 1193180 / hz;
	outb(0x43,0x36);
	outb(0x40,divisor & 0xFF);
	outb(0x40,(divisor >> 8) & 0xFF);
}

static void timer_handler(struct regs* r)
{
	timer_ticks++;
}

void timer_init()
{
	irq_install_handler(0,timer_handler);
	timer_phase(TICKS_PER_SEC);
}

void timer_wait(int ticks)
{
	uint32_t tks = timer_ticks + ticks;
	while(timer_ticks < tks);
}

uint32_t clock()
{
	return timer_ticks;
}