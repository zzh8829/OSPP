#ifndef _UTIL_H_
#define _UTIL_H_
 
#include "system.h"

typedef void* type_t;

typedef uint8_t (*less_than_f)(type_t,type_t);

typedef struct 
{
	type_t *array;
	uint32_t size;
	uint32_t max_size;
	less_than_f less_than;
} ordered_array_t;

uint8_t standard_less_than(type_t a, type_t b);

ordered_array_t ordered_array_create (uint32_t max_size,less_than_f less_than);
ordered_array_t ordered_array_place (void* addr, uint32_t max_size,less_than_f less_than);

void ordered_array_destroy(ordered_array_t* array);
void ordered_array_insert(ordered_array_t* array,type_t item);
type_t ordered_array_get(ordered_array_t* array, uint32_t idx);
void ordered_array_remove(ordered_array_t* array, uint32_t idx);
 
#endif