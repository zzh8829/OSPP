#ifndef _FONT_H_
#define _FONT_H_
 
#include "system.h"
#include "graphics.h"

extern uint32_t font_w, font_h;
extern uint32_t* font_glyph[128];

surface_t* font_render(char* str,color_t color);
void font_init();

#endif