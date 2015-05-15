#include "system.h"

#define GDT_SIZE 5
#define NULL_SEL 0
#define CODE_SEL 8
#define DATA_SEL 16
#define TSS_SEL 24
#define LDT_SEL 32

#define USER_CODE_SEL 7
#define USER_DATA_SEL 15
/* Defines a GDT entry. We say packed, because it prevents the
*  compiler from doing things that it thinks is best: Prevent
*  compiler "optimization" by packing */
struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct gdt_entry gdt[GDT_SIZE];
struct gdt_ptr gp;

/* This will be a function in start.asm. We use this to properly
*  reload the new segment registers */
void gdt_load();
void gdt_init();