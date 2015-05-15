#ifndef _HEAP_H_
#define _HEAP_H_

#include "system.h"
#include "util.h"

#define KHEAP_START			0x30000000
#define KHEAP_INITIAL_SIZE	0x10000000
#define KHEAP_MAX_ADDRESS	0x30000000

#define HEAP_INDEX_SIZE		0x00020000
#define HEAP_MAGIC			0x123890AB
#define HEAP_MIN_SIZE		0x00070000

typedef struct
{
	uint32_t magic;   // Magic number, used for error checking and identification.
	uint8_t is_hole;   // 1 if this is a hole. 0 if this is a block.
	uint32_t size;	// size of the block, including the end footer.
} header_t;

typedef struct
{
	uint32_t magic;	 // Magic number, same as in header_t.
	header_t *header; // Pointer to the block header.
} footer_t;

typedef struct
{
	ordered_array_t index;
	uint32_t start_address; // The start of our allocated space.
	uint32_t end_address;   // The end of our allocated space. May be expanded up to max_address.
	uint32_t max_address;   // The maximum address the heap can be expanded to.
	uint8_t supervisor;	 // Should extra pages requested by us be mapped as supervisor-only?
	uint8_t readonly;	   // Should extra pages requested by us be mapped as read-only?
} heap_t;

heap_t *kheap;
uint32_t placement_address;

void heap_init();

heap_t *heap_create(uint32_t start, uint32_t end, uint32_t max, uint8_t supervisor, uint8_t readonly);
void *heap_alloc(uint32_t size, uint8_t page_align, heap_t *heap);
void heap_free(void *p, heap_t *heap);


uint32_t physical_address(void* virt);

uint32_t kmalloc(uint32_t size); // Standard

uint32_t kamalloc(uint32_t size); // Align
uint32_t kpmalloc(uint32_t size, uint32_t* phys); // Address
uint32_t kzmalloc(uint32_t size); // Zero

// combined
uint32_t kapmalloc(uint32_t size, uint32_t* phys);
uint32_t kazmalloc(uint32_t size);
uint32_t kapzmalloc(uint32_t size, uint32_t* phys);

void kfree(void *p);

 
#endif