#ifndef _IRQ_H_
#define _IRQ_H_
 
#include "idt.h"

void irq_init();
 
typedef void (*irq_handler_t) (struct regs *);
void irq_handler(struct regs*);
void irq_install_handler(size_t irq, irq_handler_t handler);
void irq_uninstall_handler(size_t irq);
irq_handler_t irq_routines[16];


#endif