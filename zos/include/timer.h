#ifndef _TIMER_H_
#define _TIMER_H_
 
#include "system.h"

void timer_init();
void timer_wait(int);
uint32_t clock();
 
uint32_t TICKS_PER_SEC;

#endif