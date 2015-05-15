#include "gdt.h"

void gdt_set(int num, uint32_t base, uint32_t limit,uint8_t access,uint8_t gran)
{
    /* Setup the descriptor base address */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void gdt_init()
{
   /* Setup the GDT pointer and limit */
    gp.limit = (sizeof(struct gdt_entry) * GDT_SIZE) - 1;
    gp.base = (uint32_t)&gdt;
    gdt_set(0, 0, 0, 0, 0);
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // CODE
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // DATA
    gdt_load();
}