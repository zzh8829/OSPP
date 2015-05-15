#include "kstdio.h"

struct KPL_STRUCT 
{
	COLOR fg;
	COLOR bg;
} KP_DETAIL[] = 
{
	{BRIGHT_WHITE, 		BLACK	},
	{YELLOW,			RED		},
};

static void parse_num(uint32_t value, uint32_t base, uint32_t len, char* buf, int* ptr)
{
	uint32_t n = value/base;
	int r = value % base;
	if(r<0) 
	{
		r += base;
		--n;
	}
	if(value >= base)
		parse_num(n,base,len,buf,ptr);
	buf[(*ptr)++] = "0123456789"[r];
}

static void parse_hex(uint32_t value,uint32_t len, char* buf, int* ptr) 
{
	int i = len?len:8;
	while(i-- > 0)
	{
		buf[(*ptr)++] = "0123456789abcdef"[(value>>(i*4))&0xf];
	}
}

int vsprintf(char* buf,const char *fmt, va_list args) 
{
	int i = 0;
	char *s;
	int ptr = 0;
	int len = strlen(fmt);
	for (; i<len && fmt[i]; ++i) 
	{
		if ((fmt[i]!='%') && (fmt[i]!='\\')) 
		{
			buf[ptr++] = fmt[i];
			continue;
		} 
		if (fmt[i] == '\\') 
		{
			/* \a \b \t \n \v \f \r \\ */
			switch (fmt[++i]) 
			{
				case 'a': buf[ptr++] = '\a'; break;
				case 'b': buf[ptr++] = '\b'; break;
				case 't': buf[ptr++] = '\t'; break;
				case 'n': buf[ptr++] = '\n'; break;
				case 'r': buf[ptr++] = '\r'; break;
				case '\\':buf[ptr++] = '\\'; break;
			}
			continue;
		}
		uint32_t arg_width = 0;
		while (fmt[++i] >= '0' && fmt[i] <= '9') {
			arg_width *= 10;
			arg_width += fmt[i] - '0';
		}
		/* fmt[i] == '%' */
		switch (fmt[i]) 
		{
			case 's':
				s = (char *)va_arg(args, char *);
				while (*s)
					buf[ptr++] = *s++;
				break;
			case 'c':
				/* why is int?? */
				buf[ptr++] = (char)va_arg(args, int);
				break;
			case 'x':
				parse_hex((uint32_t)va_arg(args, uint32_t),arg_width,buf,&ptr);
				break;
			case 'd':
				parse_num((uint32_t)va_arg(args, uint32_t),10,arg_width,buf,&ptr);
				break;
			case 'b':
				parse_num((uint32_t)va_arg(args, uint32_t),2, arg_width,buf,&ptr);
				break;
			case '%':
				buf[ptr++] = '%';
				break;
			default:
				buf[ptr++] = fmt[i];
				break;
		}
	}
	buf[ptr] = '\0';
	return ptr;
	
}	

int kprintf(enum KP_LEVEL kl,const char *fmt, ...) 
{
	char buf[1024]= {-1};	
	va_list args;
	va_start(args, fmt);
	int len = vsprintf(buf, fmt, args);
	va_end(args);

	int i;
	for (i=0; i<len; ++i)
		_print_c(buf[i], KP_DETAIL[kl].fg, KP_DETAIL[kl].bg);

	int x,y;
	get_cursor(&x,&y);
	set_cursor(x,y);

	return len;
}

int _kprintf(enum KP_LEVEL kl,const char *fmt, ...) 
{
	char buf[1024]= {-1};	
	va_list args;
	va_start(args, fmt);
	int len = vsprintf(buf, fmt, args);
	va_end(args);
	int i;
	for (i=0; i<len; ++i)
		_print_c(buf[i], KP_DETAIL[kl].fg, KP_DETAIL[kl].bg);

	return len;
}

int sprintf(char* buf, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int len = vsprintf(buf, fmt, args);
	va_end(args);
	return len;
}

int kp(const char *fmt, ...) 
{
	char buf[1024]= {-1};
	va_list args;
	va_start(args, fmt);
	int len = vsprintf(buf, fmt, args);
	va_end(args);

	int i;
	buf[len++]='\n';
	for (i=0; i<len; ++i)
		_print_c(buf[i], KP_DETAIL[DUMP].fg, KP_DETAIL[DUMP].bg);

	int x,y;
	get_cursor(&x,&y);
	set_cursor(x,y);

	return len;
}

void __assert(const char* msg, const char* file, int line)
{
	kprintf(PANIC,"ERROR: %s at %s line %d\n",msg,file,line);	
	for(ever);
}