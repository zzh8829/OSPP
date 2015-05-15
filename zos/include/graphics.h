#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
 
#include "system.h"
#include "vesa.h"

uint8_t* video_memory;

// w,h in pixel
int screen_width,screen_height;

// size of screen in int8
int screen_size;

// bit Per Pixel, Int8 Per Pixel
int screen_u1pp,screen_u8pp;

// Pixel Per Line, Int8 Per Line
int screen_ppl,screen_u8pl;

typedef union
{
	uint32_t value;
	struct 
	{
		uint8_t b; // 0-8
		uint8_t g; // 8-16
		uint8_t r; // 16-24
		uint8_t a; // 24-32
	};
} color_t;

typedef struct
{
	int x,y;
} point_t;

typedef struct
{
	int x,y,w,h;
} rect_t;

typedef struct
{
	union
	{
		struct
		{
			int x,y,w,h;
		};
		rect_t rect;
	};
	
	int u8pp;
	int u8pl;

	int buffer_size;
	uint8_t* buffer;
} surface_t;

extern surface_t* g_screen;

uint8_t vbuf[1920*1080*4];

color_t rgba(int r,int g,int b,int a);

color_t rgb(int r,int g,int b);
// 0-360, 0-1, 0-1
color_t hsl(int h,float s, float l);

void surface_blit(surface_t* src,rect_t* srcrect,surface_t* dst,rect_t* dstrect);
void surface_blend(surface_t* src,rect_t* srcrect,surface_t* dst,rect_t* dstrect);
void surface_fill(surface_t* surface, rect_t* rect, color_t color);
surface_t* surface_create(int w,int h);
void surface_free(surface_t* surface);

void fillrect_buf(int x,int y,int w,int h, uint8_t* buf);
void fillrect(int x,int y,int w,int h,uint32_t color);
void drawline(int x1,int y1,int x2,int y2, uint32_t color);
//void setpixel(int x,int y,uint32_t color);
void setpixel(uint32_t pos, uint32_t color);

void flip();
void clear_screen();

void graphics_init(vbe_info_t* vbe);

#endif
