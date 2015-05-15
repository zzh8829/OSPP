#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#define VIDEO_MEMORY 0xb8000
#define MAX_ROW 25
#define MAX_COL 80

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define GRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

#if 0
static uint8_t vbuf[2*25*80];

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
	__asm__("in %%dx, %%ax" 
		: "=a" (result) 
		: "d" (port)
	);
	return result;
}

void outw(uint16_t port, uint16_t data)
{
	__asm__("out %%ax, %%dx"
		: /* no input */
		: "a" (data), "d" (port)
	);
}

int get_cursor(void)
{
	outb(0x3d4, 0xe);
	int offset = inb(0x3d5) << 8;
	outb(0x3d4, 0xf);
	offset += inb(0x3d5);
	return offset*2;
}

void set_cursor(int offset)
{
	offset /= 2;
	outb(0x3d4,0xe);
	outb(0x3d5,(uint8_t)((offset>>8)&0xff));
	outb(0x3d4,0xf);
	outb(0x3d5,(uint8_t)(offset&0xff));
}

int get_screen_offset(int row,int col)
{
	return 2* (row*MAX_COL + col);
}

void cls(void)
{
	int i;
	for(i=0;i!=80*25;i++)
	{
		vbuf[i*2] = 0;
		vbuf[i*2+1] = 0;
	}
}

void print_char(char ch, int row, int col, uint8_t attr)
{
	int offset;
	if(col >= 0 && row >= 0)
	{
		offset = get_screen_offset(row, col);
	} else {
		offset = get_cursor();
	}

	if(ch =='\n')
	{
		int nrow = offset / (2*MAX_COL);
		offset = get_screen_offset(nrow,79);
	} else {
		vbuf[offset] = ch;
		vbuf[offset+1] = attr;
	}

	offset += 2;

	set_cursor(offset);
}

void putchar_at(char ch, int row, int col)
{
	if(row >= 0 && col >=0)
	{
		set_cursor(get_screen_offset(row,col));
	}
	print_char(ch, row, col, 0xf);
}

void putchar(char ch)
{
	putchar_at(ch,-1,-1);
}

void puts(char* str)
{
	while((*str)!='\0')
	{
		putchar(*str);
		str++;
	}
}

void setpixel(int x,int y,uint8_t color)
{
	vbuf[2*(y*80+x)] = 0;
	vbuf[2*(y*80+x)+1] = (color << 4)|0;
}

void fillrect(int x,int y,int w, int h, uint8_t color)
{
	color = (color << 4) | 0;
	int i,j;
	for(i=y;i<y+h;i++)
	{
		for(j=x;j<x+w;j++)
		{
			vbuf[2*(i*80+j)] = 0;
			vbuf[2*(i*80+j)+1] = color;
		}
	}
}

void flipbuffer(void)
{
	uint8_t * vm = (uint8_t*)VIDEO_MEMORY;
	int i;
	for(i=0;i!=80*25;i++)
	{
		*(vm+i*2) = vbuf[i*2];
		*(vm+i*2+1) = vbuf[i*2+1];
	}
}
#endif 

void main(void)
{
	*(uint8_t*)0xb8000 = 'X';
	/*
	cls();
	set_cursor(0);
	fillrect(1,8,20,14,GREEN);
	fillrect(10,10,40,10,BLUE);
	fillrect(30,5,40,19,RED);
	puts("Hello\n");
	puts("This is a simple kernel written in c\n");
	puts("HOOOOOOO YAAAAAA HAHAHHHHHAHHAHAHA\n");
	puts("LOLOLOLOL HELLO HELLO HELLO HELLO");
	flipbuffer();
	*/
}
