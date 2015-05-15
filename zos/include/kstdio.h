#ifndef _K_STDIO_H_
#define _K_STDIO_H_ 1

#include "system.h"
#include "kconsole.h"

#define ASSERT(EX) (void)((EX) || (__assert (#EX, __FILE__, __LINE__),0))

void __assert(const char* msg, const char* file, int line);

/*
#define args_list uint8_t*
#define _args_stack_size(type) \
	(((sizeof(type)-1)/sizeof(int)+1)*sizeof(int))

#define args_start(ap,fmt) do \
	{\
		ap = (uint8_t*)((uint32_t)&fmt + _args_stack_size(&fmt)); \
	} while(0)

#define args_end(ap)

#define args_next(ap, type) \
	(((type*)(ap+=_args_stack_size(type)))[-1])

*/
typedef __builtin_va_list va_list;
#define va_start(ap,last) __builtin_va_start(ap, last)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap,type) __builtin_va_arg(ap,type)
#define va_copy(dest, src) __builtin_va_copy(dest,src)

enum KP_LEVEL
{
	DUMP,
	PANIC
};

int kprintf(enum KP_LEVEL kl, const char* fmt, ...);
int sprintf(char* buf, const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list args);
int _kprintf(enum KP_LEVEL kl, const char* fmt, ...);
int kp(const char* fmt, ...);


#endif