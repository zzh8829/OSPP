#include "paging.h"
#include "kstdio.h"
#include "isrs.h"
#include "heap.h"
#include "cpu.h"
uint32_t mem_end;

uint32_t *frames;
uint32_t nframes;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))


void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1<<off);
}

static void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1<<off);
}

static uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1<<off));
}

static uint32_t get_frame()
{
	uint32_t i,j;
	for(i=0;i<INDEX_FROM_BIT(nframes); i++)
	{
		if(frames[i] != 0xFFFFFFFF)
		{
			for(j=0;j<32;j++)
			{
				uint32_t bit = 0x1<<j;
				if(!(frames[i]&bit))
				{
					return i*4*8+j;
				}
			}
		}
	}
	return 0;
}

void alloc_frame(page_t *page, int kernel, int writeable)
{
	if(page->frame!=0)
	{
		return;
	}
	uint32_t idx = get_frame();
	set_frame(idx*0x1000);
	page->present = 1;
	page->rw = writeable?1:0;
	page->user = kernel?0:1;
	page->frame = idx;	
}

void free_frame(page_t *page)
{
	uint32_t frame = page->frame;
	if(!frame)
	{
		return;
	}
	clear_frame(frame*0x1000);
	page->frame = 0x0;
}

void page_fault (struct regs *r)
{
	uint32_t cr2;
	__asm__ volatile("mov %%cr2, %0" : "=r"(cr2));

	int present   = !(r->err_code & 0x1); // Page not present
    int rw = r->err_code & 0x2;           // Write operation?
    int us = r->err_code & 0x4;           // Processor was in user-mode?
    int reserved = r->err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    //int id = r.err_code & 0x10;          // Caused by an instruction fetch?

    kp("Page fault!");
    if (present) {kp("page not present ");}
    if (rw) {kp("read-only ");}
    if (us) {kp("user-mode ");}
    if (reserved) {kp("reserved ");}
    kp("at 0x%x - EIP: %x",cr2,r->eip);
    
    if(!strcmp((char*)r->eip,(char*)cr2))
         kp("Page fault caused by executing unpaged memory");
    else
         kp("Page fault caused by reading unpaged memory");

    sti();
	for(ever);
}

void switch_page_directory(page_directory_t* pd)
{
	current_directory = pd;
	__asm__ volatile ("mov %0, %%cr3":: "r"(pd->physical_address));
	uint32_t cr0;
	__asm__ volatile ("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ volatile ("mov %0, %%cr0":: "r"(cr0));
}

page_t* get_page(uint32_t address, int make ,page_directory_t* dir)
{
	address /= 0x1000;
	uint32_t table_idx = address/1024;
	if(dir->tables[table_idx])
	{
		return &dir->tables[table_idx]->pages[address%1024];
	}
	if(make)
	{
		uint32_t tmp;
		dir->tables[table_idx]=(page_table_t*)kapmalloc(sizeof(page_table_t),&tmp);
		memset(dir->tables[table_idx],0,0x1000);
		dir->physical_tables[table_idx] = tmp|0x7;
		return &dir->tables[table_idx]->pages[address%1024];
	}
	return 0;
}

void dma_address(uint32_t address)
{
	if(address < mem_end)
		set_frame(address);
	page_t *page = get_page(address, 1, kernel_directory);
	page->present = 1;
	page->rw = 1;
	page->user = 1;
	page->frame = address/0x1000;
}

void dma_area(uint32_t address, uint32_t length)
{
	address = (address/0x1000)*0x1000;
	length = (length/0x1000+1)*0x1000;
	for(int i=0;i!=length;i+=0x1000 )
		dma_address(address+i);
}

void paging_init(uint32_t memory_size)
{
	extern uint32_t end;
	placement_address = (uint32_t)&end;
	kheap = 0;

	kp("Paging Initialzing");
	mem_end = memory_size*1024;

	nframes = mem_end / 0x1000;
	frames = (uint32_t*)kzmalloc(INDEX_FROM_BIT(nframes*8));
	//memset(frames,0,INDEX_FROM_BIT(nframes));
	kernel_directory = (page_directory_t*)kazmalloc(sizeof(page_directory_t));
	//memset(kernel_directory,0,sizeof(page_directory_t));
	kernel_directory->physical_address = (uint32_t)kernel_directory->physical_tables;
	current_directory = kernel_directory;
	size_t i;

	for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
		get_page(i, 1, kernel_directory);

	i = 0;
	while (i < placement_address+0x1000)
	{
		// Kernel code is readable but not writeable from userspace.
		alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}
		
	for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
		alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

	isrs_install_handler(14,page_fault);
	
	switch_page_directory(kernel_directory);
}

