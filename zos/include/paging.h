#ifndef _PAGING_H_
#define _PAGING_H_
 
#include "system.h"

typedef struct page
{
    uint32_t present    : 1;   // Page present in memory
    uint32_t rw         : 1;   // Read-only if clear, readwrite if set
    uint32_t user       : 1;   // Supervisor level only if clear
    uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
    uint32_t dirty      : 1;   // Has the page been written to since last refresh?
    uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
    uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
}__attribute__((packed)) page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t* tables[1024];
	uint32_t physical_tables[1024];
	uint32_t physical_address;
} page_directory_t;

page_directory_t* kernel_directory;
page_directory_t* current_directory;

extern uint32_t mem_end;

void paging_init(uint32_t memory_size);

void switch_page_directory (page_directory_t *pd);
page_t* get_page( uint32_t address, int make , page_directory_t* dir);
void set_frame(uint32_t frame_addr);

void alloc_frame(page_t *page, int kernel, int writeable);
void free_frame(page_t *page);

void dma_address(uint32_t address);
void dma_area(uint32_t address, uint32_t length);

#endif