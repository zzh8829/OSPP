#include "vbe.h"

void init_graphics(uint8_t* _vbuf)
{
	vbeModeInfo = (struct VBEModeInfo*)0x3000;
	vbuf = _vbuf;	
}

void set_pixel(int x,int y,int color)
{
	int pos = x * vbeModeInfo->bpp + y * vbeModeInfo->pitch;
	vbuf[pos] = color&255;
	vbuf[pos+1] = (color >> 8) & 255;
	vbuf[pos+2] = (color >> 16) & 255;
}