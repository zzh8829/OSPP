#ifndef _ISRS_H_
#define _ISRS_H_

#include "idt.h"

void isrs_init();

typedef void (*isrs_handler_t) (struct regs *);
void fault_handler(struct regs*);
void isrs_install_handler(size_t isrs, isrs_handler_t handler);
void isrs_uninstall_handler(size_t isrs);
isrs_handler_t isrs_routines[256];
 
#endif