#include "util.h"
#include "heap.h"

uint8_t standard_less_than(type_t a, type_t b)
{
	return (a<b)?1:0;
}

ordered_array_t ordered_array_create(uint32_t max_size,less_than_f less_than)
{
	ordered_array_t array;
	array.array = (void*)kzmalloc(max_size*sizeof(type_t));
	array.size = 0;
	array.max_size = max_size;
	array.less_than = less_than;
	return array;
}

ordered_array_t ordered_array_place(void* addr,uint32_t max_size,less_than_f less_than)
{
	ordered_array_t array;
	array.array = (type_t*)addr;
	array.size = 0;
	array.max_size = max_size;
	array.less_than = less_than;
	return array;
}

void ordered_array_destroy(ordered_array_t* array)
{
	// Not Implemented
}

void ordered_array_insert(ordered_array_t* array, type_t item)
{
	uint32_t idx = 0;
	while(idx < array->size && array->less_than(array->array[idx],item))
		idx++;
	if(idx == array->size)
		array->array[array->size++] = item;
	else
	{
		type_t tmp = array->array[idx];
		array->array[idx] = item;
		while(idx<array->size)
		{
			idx++;
			type_t tmp2 = array->array[idx];
			array->array[idx] = tmp;
			tmp = tmp2;
		}
		array->size++;
	}
}

type_t ordered_array_get(ordered_array_t* array, uint32_t idx)
{
	return array->array[idx];
}

void ordered_array_remove(ordered_array_t* array, uint32_t idx)
{
	while(idx < array->size)
	{
		array->array[idx] = array->array[idx+1];
		idx++;
	}
	array->size--;
}