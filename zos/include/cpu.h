#ifndef _CPU_H_
#define _CPU_H_
 
#include "system.h"

void cli();     /* Disable ints */
void sti();     /* Enable ints */
void hlt();    /* Halts CPU */
void reboot();  /* Reboots CPU */

uint8_t read_CMOS(uint8_t addr);             /* Get CMOS data */
void write_CMOS(uint8_t addr, uint8_t value); /* Write CMOS data */

#endif