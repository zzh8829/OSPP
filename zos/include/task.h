#ifndef _TASK_H_
#define _TASK_H_
 
#include "system.h"

#define TASK_RUNNING	0
#define TASK_RUNABLE	1
#define TASK_STOPPED	2

#define DEFAULT_LDT_CODE	0x00cffa000000ffffULL
#define DEFAULT_LDT_DATA	0x00cff2000000ffffULL

#define INITIAL_PRIORITY	200

typedef struct task_t
{
	tss_t tss;
	uint64_t tss_entry;
	uint64_t ldt[2];
	uint64_t ldt_entry;
	int state;
	int priority;
	struct task_t* next;
} task_t;

task_t *current_task;

 
#endif