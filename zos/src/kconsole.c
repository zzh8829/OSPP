#include "kconsole.h"
#include "kstdio.h"
static int curx=0;
static int cury=0;
static int serial = 0;

void open_serial_output()
{   
    outb(0x3f8 + 1, 0x00);
    outb(0x3f8 + 3, 0x80);
    outb(0x3f8 + 0, 0x03);
    outb(0x3f8 + 1, 0x00);
    outb(0x3f8 + 3, 0x03);
    outb(0x3f8 + 2, 0xC7);
    outb(0x3f8 + 4, 0x0B);
    serial = 1;
}

void close_serial_output()
{
	serial = 0;
}

void scroll(int lines)
{
	int16_t *p = (int16_t*)(VIDEO_MEM+CHAR_OFF(MAX_COL-1,MAX_ROW-1));
	int i = MAX_COL-1;
	memcpy((void*)VIDEO_MEM, (void*)(VIDEO_MEM+ROW_SIZE*lines), ROW_SIZE*(MAX_ROW-lines));
	for(;i>=0;i--)
		*p-- = (int16_t)((BLANK_ATTR<<8)|BLANK_CHAR);
}

void clear_console()
{
	memsetw((uint16_t*)VIDEO_MEM,(BLANK_ATTR<<8)|BLANK_CHAR,PAGE_SIZE);
	set_cursor(0,0);
}

void _set_cursor(int x,int y)
{
	curx = x;
	cury = y;
}

void set_cursor(int x,int y)
{
	_set_cursor(x,y);
	uint16_t pos = (y*80)+x;
	outb(0x3d4, 0x0e);
	outb(0x3d5, (uint8_t)((pos>>8)&0xff));
	outb(0x3d4, 0x0f);
	outb(0x3d5, (uint8_t)(pos&0xff));
}

void get_cursor(int* x,int * y)
{
	*x = curx;
	*y = cury;
}

void _print_c(char c, COLOR fg, COLOR bg)
{
	if(serial==1)
	{
		while ((inb(0x3f8 + 5) & 0x20) == 0);
    	outb(0x3f8, c&0x7f);
	}
	char *p;
	char attr;
	p = (char *) VIDEO_MEM + CHAR_OFF(curx,cury);
	attr = (char)(bg<<4|fg);
	switch(c)
	{
		case '\r':
			curx = 0;
			break;
		case '\n':
			for(; curx<MAX_COL; curx++)
			{
				*p++ = BLANK_CHAR;
				*p++ = attr;
			}
			break;
		case '\t':
			c = curx+TAB_SIZE-(curx&(TAB_SIZE-1));
			c = c<MAX_COL?c:MAX_COL;
			for (; curx<c; ++curx) {
			        *p++ = BLANK_CHAR;
			        *p++ = attr;
			}
			break;
		default:
			*p++ = c;
			*p++ = attr;
			curx++;
			break;
	}
	if(curx >= MAX_COL)
	{
		curx = 0;
		if(cury < MAX_ROW-1)
			cury++;
		else
			scroll(1);
	}
}

void print_c(char c, COLOR fg, COLOR bg)
{
 	_print_c(c, fg, bg);
	set_cursor(curx,cury);
}