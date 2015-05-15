#include "task.h"
#include "gdt.h"

static uint32_t task0_stack[256] = {0};

task_t task0 = {
	/* tss */
	.tss = {	/* back_link */
		0,
		/* esp0                                    ss0 */
		(unsigned)&task0_stack+sizeof(task0_stack), DATA_SEL, 
		/* esp1 ss1 esp2 ss2 */
		0, 0, 0, 0, 
		/* cr3 */
		0, 
		/* eip eflags */
		0, 0, 
		/* eax ecx edx ebx */
		0, 0, 0, 0,
		/* esp ebp */
		0, 0,
		/* esi edi */
		0, 0, 
		/* es          cs             ds */
		USER_DATA_SEL, USER_CODE_SEL, USER_DATA_SEL, 
		/* ss          fs             gs */
		USER_DATA_SEL, USER_DATA_SEL, USER_DATA_SEL, 
		/* ldt */
		0x20,
		/* trace_bitmap */
		0x00000000
	},
		/* tss_entry */
	0,
	/* idt[2] */
	{DEFAULT_LDT_CODE, DEFAULT_LDT_DATA},
	/* idt_entry */
	0,
	/* state */
	TASK_RUNNING,
	/* priority */
	INITIAL_PRIORITY,
	/* next */
	0,
};