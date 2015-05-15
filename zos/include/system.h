#ifndef _SYSTEM_H_
#define _SYSTEM_H_ 1

#include <stdbool.h> 
#include <stddef.h>
#include <stdint.h>

typedef struct regs 
{
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
}__attribute__((packed)) reg_t;

typedef struct 
{
        uint32_t back_link;
        uint32_t esp0, ss0;
        uint32_t esp1, ss1;
        uint32_t esp2, ss2;
        uint32_t cr3;
        uint32_t eip;
        uint32_t eflags;
        uint32_t eax,ecx,edx,ebx;
        uint32_t esp, ebp;
        uint32_t esi, edi;
        uint32_t es, cs, ss, ds, fs, gs;
        uint32_t ldt;
        uint32_t trace_bitmap;
}__attribute__((packed)) tss_t;

void bcopy(const void* src, void* dest,uint32_t n);
void bzero(void* dest, uint32_t n);
uint8_t* memcpy(void* dest, const void* src,uint32_t n);
void* memcpyw(void* dest, const void* src,uint32_t n);
uint32_t* memcpyl(uint32_t* dest, const uint32_t* src,uint32_t n);
uint8_t* memset(void* dest, uint8_t val, uint32_t n);
uint16_t* memsetw(uint16_t* dest, uint16_t val, uint32_t n);
uint32_t* memsetl(uint32_t* dest, uint32_t val, uint32_t n);
int strlen(const char* str);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t data);

int abs(int n);
float fabs(float n);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define ever ;;
#define how_long_i_will_love_you for(ever) 

//typedef size_t uint32_t     
int strcmp(const char * s1, const char * s2);
int strncmp(const char * s1, const char* s2, size_t n);


#endif
