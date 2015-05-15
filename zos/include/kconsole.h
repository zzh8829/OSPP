#ifndef _K_CONSOLE_H_
#define _K_CONSOLE_H_ 1

#include "system.h"

#define MAX_ROW		25
#define MAX_COL 	80
#define TAB_SIZE	4			/* must be power of 2 */

/* color text mode, the video ram starts from 0xb8000,
   we all have color text mode, right? :) */
#define VIDEO_MEM	0xb8000

#define ROW_SIZE	(MAX_COL*2)

#define PAGE_SIZE	(MAX_ROW*MAX_COL)

#define BLANK_CHAR	(' ')
#define BLANK_ATTR	(0x07)		/* white fg, black bg */

#define CHAR_OFF(x,y)	(ROW_SIZE*(y)+2*(x))

typedef enum COLOR_TAG {
	BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
	GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, 
	LIGHT_RED, LIGHT_MAGENTA, YELLOW, BRIGHT_WHITE
} COLOR;

void set_cursor(int, int);
void get_cursor(int *, int *);
void _set_cursor(int, int);
void print_c(char, COLOR, COLOR);
void _print_c(char, COLOR, COLOR);
void scroll(int lines);
void clear_console();


void open_serial_output();
void close_serial_output();

#endif