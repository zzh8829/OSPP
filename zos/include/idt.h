#ifndef _IDT_H_
#define _IDT_H_
 
#include "system.h"

/* Defines an IDT entry */
struct idt_entry
{
    uint16_t base_low;
    uint16_t sel;        /* Our kernel segment goes here! */
    uint8_t zero;     /* This will ALWAYS be set to 0! */
    uint8_t flags;       /* Set using the above table! */
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set(uint8_t num, uint32_t base, uint16_t sel,uint8_t flags);
void idt_load();
void idt_init();
 
#endif