#include "system.h"

// Memory functions may contain massive Assembly Hack
// This is about 100% faster(2x) than naive C operation
// It works I have no idea why... copy paste u know :P

/*
void* memcpy(void* dest, const void* src,uint32_t n)
{
	uint8_t * dp = (uint8_t*)dest;
	const uint8_t* sp = (const uint8_t*)src;
	while(n--)
		*dp++ = *sp++;
	return dest;

}
*/

uint8_t* memcpy(void* dest, const void* src,uint32_t n)
{
	uint32_t dp = (uint32_t)dest;
	uint32_t sp = (uint32_t)src;
	if( (dp & 3) != (sp & 3) )
	{
		__asm__ __volatile__ ("rep movsb" :: "D" (dp), "S" (sp), "c" (n));
	} else {
		__asm__ __volatile__ (
			"rep movsl;\n\t"
			"mov %3, %%ecx;\n\t"
			"rep movsb"
			:: "D" (dest), "S" (src), "c" (n/4), "r" (n&3));
	}
	return dest;
}

uint32_t* memcpyl(uint32_t* dest, const uint32_t* src,uint32_t n)
{
	__asm__ __volatile__ ("rep movsl" :: "D" (dest), "S" (src), "c" (n));
	return dest;
}

uint8_t* memset(void* dest, uint8_t val, uint32_t n)
{
	uint8_t* dp = (uint8_t*)dest;
	while(n--) *dp++ = val;
	return dest;
}

uint16_t* memsetw(uint16_t* dest, uint16_t val, uint32_t n)
{
	uint16_t* dp = dest;
	while(n--) *dp++ = val;
	return dest;
}

uint32_t* memsetl(uint32_t* dest, uint32_t val, uint32_t n)
{
	__asm__ __volatile__ ("rep stosl" :: "D" (dest), "a" (val), "c" (n));
	return dest;
}

void bcopy(const void* src, void* dest,uint32_t n)
{
	memcpy(dest,src,n);
}

void bzero(void* dest, uint32_t n)
{
	memset(dest,0,n);
}

int strlen(const char* str)
{
	int n = 0;
	while(*str++)n++;
	return n;
}

uint8_t inb(uint16_t port)
{
	uint8_t result;
	__asm__("in %%dx, %%al"
		: "=a" (result) 
		: "d" (port)
	);
	return result;
}

void outb(uint16_t port, uint8_t data)
{
	__asm__("out %%al, %%dx"
		: /* no input */
		: "a" (data), "d" (port)
	);
}

uint16_t inw(uint16_t port)
{
	uint16_t result;
	__asm__("inw %%dx, %%ax" 
		: "=a" (result) 
		: "d" (port)
	);
	return result;
}

void outw(uint16_t port, uint16_t data)
{
	__asm__("outw %%ax, %%dx"
		: /* no input */
		: "a" (data), "d" (port)
	);
}

uint32_t inl(uint16_t port)
{
	uint32_t result;
	__asm__("inl %%dx, %%eax" 
		: "=a" (result) 
		: "d" (port)
	);
	return result;
}

void outl(uint16_t port, uint32_t data)
{
	__asm__("outl %%eax, %%dx"
		: /* no input */
		: "a" (data), "d" (port)
	);
}

int abs(int n)
{
	return n>0?n:-n;
}

float fabs(float n)
{
	return n>0?n:-n;
}

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int strncmp(const char * s1, const char* s2, size_t n)
{
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}