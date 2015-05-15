#include "graphics.h"
#include "kstdio.h"
#include "paging.h"
#include "heap.h"
#include "font.h"

surface_t* g_screen;

#define SURFACE_BUFFER_OFFSET(surf,x,y) \
	((uint8_t*)(surf->buffer) + (x)*(surf->u8pp) + (y)*(surf->u8pl))

color_t rgba(int r,int g,int b,int a)
{
	color_t c = {((a<<24)|(r<<16)|(g<<8)|b)};
	return  c;
}

color_t rgb(int r,int g,int b)
{
	return rgba(r,g,b,255);
}

// 0-360, 0-1, 0-1
color_t hsl(int h,float s, float l)
{
	float c = (1-fabs(2*l-1))*s;
	float hh = h/60.0;
	while(hh>2)hh-=2;
	float x = c*(1-fabs(hh-1));
	float m = l-c/2;
	float r=0,g=0,b=0;
	switch(h/60)
	{
		case 0:r = c;g = x;break;
		case 1:r = x;g = c;break;
		case 2:g = c;b = x;break;
		case 3:g = x;b = c;break;
		case 4:r = x;b = c;break;
		case 5:r = c;b = x;break;
	}
	r+=m;g+=m;b+=m;
	return rgb(r*255,g*255,b*255);
}
#if 0
void fillrect(int x,int y,int w,int h, uint32_t color)
{
	x = MAX(MIN(x,screen_width),0);
	y = MAX(MIN(y,screen_height),0);
	w = MAX(MIN(w,screen_width-x),x-screen_width);
	h = MAX(MIN(h,screen_height-y),y-screen_height);

	int i,j,dx=1,dy=1;
	if(w<0)dx=-1;
	if(h<0)dy=-1;
	
	int pos = y*screen_width+x;
	for(i=0;i!=h;i+=dy)
	{
		for(j=0;j!=w;j+=dx)
		{
			setpixel(pos,color);
			pos+=dx;
		}
		pos+= dy*screen_width-w;
	}
}

void drawline(int x0,int y0, int x1, int y1, uint32_t color)
{
	x0 = MAX(MIN(screen_width,x0),x0);
	x1 = MAX(MIN(screen_width,x1),x1);
	y0 = MAX(MIN(screen_height,y0),y0);
	y1 = MAX(MIN(screen_height,y1),y1);

	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
		setpixel(x0+y0*screen_ipl,color);
		if (x0==x1 && y0==y1) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

inline void setpixel(uint32_t pos,uint32_t color)
{
	*((uint32_t*)vbuf + pos) = color;
	/*
	// different bpp support, will slow down rendering speed by 10%
	pos*=4;
	switch(screen_bpp)
	{
		case 32:
		vbuf[pos + 3] = color >> 24;
		case 24:
		vbuf[pos + 2] = (color >> 16) & 0xFF;
		case 16:
		vbuf[pos + 1] = (color >> 8) & 0xFF;
		case 8:
		vbuf[pos + 0] = color & 0xFF;
		break;
	}
	*/
}
#endif

void flip()
{
	uint8_t* vm = video_memory;
	uint8_t* bm = g_screen->buffer;
	for(int i=0;i!=g_screen->h;i++)
	{
		//kp("fill %d %x",i,*(uint32_t*)bm);
		memcpyl((uint32_t*)vm,(uint32_t*)bm,g_screen->w);
		vm+=screen_u8pl;
		bm+=g_screen->u8pl;
	}
}

void graphics_init(vbe_info_t* vbe)
{
	video_memory = (uint8_t *)(vbe->physbase);
	screen_width = vbe->Xres;
	screen_height = vbe->Yres;
	screen_u1pp = vbe->bpp;
	screen_u8pp = vbe->bpp/8;
	screen_u8pl = vbe->pitch;
	screen_ppl = screen_u8pl/screen_u8pp;
	screen_size = screen_width*screen_height*screen_u8pp;

	g_screen = surface_create(screen_width,screen_height);

	surface_fill(g_screen,0,rgb(255,255,255));

	kprintf(DUMP,"Video Memory: %x\n",video_memory);
	kprintf(DUMP,"Resolution: %d x %d\n",screen_width,screen_height);
	kprintf(DUMP,"Screen BPP: %d\n", screen_u1pp);
	kprintf(DUMP,"Screen Pitch: %d\n",screen_u8pl);

	dma_area((uint32_t)video_memory,0xFF0000);
}

void surface_blit(surface_t* src,rect_t* srcrect,surface_t* dst,rect_t* dstrect)
{
	if(!srcrect) srcrect = &src->rect;
	if(!dstrect) dstrect = &dst->rect;
	
	int sx = srcrect->x;
	int sy = srcrect->y;
	int sw = srcrect->w;
	int sh = srcrect->h;

	int dx = dstrect->x;
	int dy = dstrect->y;
	int dw = dstrect->w;
	int dh = dstrect->h;

	uint8_t* sbuf = SURFACE_BUFFER_OFFSET(src,sx,sy);
	uint8_t* dbuf = SURFACE_BUFFER_OFFSET(dst,dx,dy);

	for(int i=0;i!=MIN(sh,dh);i++)
	{
		memcpy(dbuf,sbuf,MIN(sw,dw)*src->u8pp);
		sbuf += src->u8pl;
		dbuf += dst->u8pl;
	}
}

void surface_blend(surface_t* src,rect_t* srcrect,surface_t* dst,rect_t* dstrect)
{
	if(!srcrect) srcrect = &src->rect;
	if(!dstrect) dstrect = &dst->rect;
	
	int sx = srcrect->x;
	int sy = srcrect->y;
	int sw = srcrect->w;
	int sh = srcrect->h;

	int dx = dstrect->x;
	int dy = dstrect->y;
	int dw = dstrect->w;
	int dh = dstrect->h;

	uint8_t* sbuf = SURFACE_BUFFER_OFFSET(src,sx,sy);
	uint8_t* dbuf = SURFACE_BUFFER_OFFSET(dst,dx,dy);

	for(int i=0;i!=MIN(sh,dh);i++)
	{
		uint32_t* ss = (uint32_t*)sbuf;
		uint32_t* ds = (uint32_t*)dbuf;
		for(int j=0;j!=MIN(sw,dw);j++)
		{
			uint32_t dalpha,d,sl,dl;
			uint32_t s=*ss;
			uint32_t alpha = s >> 24;

			if(alpha)
			{
				if(alpha == 255)
				{
					*ds = *ss;
				} else {
					d = *ds;
					dalpha = d >> 24;
					sl = s & 0xff00ff;
					dl = d & 0xff00ff;
					dl = (dl + ((sl-dl)*alpha >> 8)) & 0xff00ff;
					s &= 0xff00;
					d &= 0xff00;
					d = (d+ ((s-d)*alpha >> 8)) & 0xff00;
					dalpha = alpha + (dalpha * (alpha ^ 0xFF) >> 8);
					*ds = dl|d|(dalpha <<24);
				}
			}
			ss++;
			ds++;
		}
		sbuf += src->u8pl;
		dbuf += dst->u8pl;
	}
}

void surface_fill(surface_t* surface, rect_t* rect, color_t color)
{
	if(!rect)rect = &surface->rect;
	uint8_t * buf = SURFACE_BUFFER_OFFSET(surface,rect->x,rect->y);
	for(int i=0;i!=rect->h;i++)
	{
		memsetl((uint32_t*)buf,color.value,rect->w);
		buf += surface->u8pl;
	}
}

void surface_set_at(surface_t* surface, point_t* p, color_t color)
{
	uint8_t* buf = SURFACE_BUFFER_OFFSET(surface,p->x,p->y);
	*((uint32_t*)buf) = color.value;
}

surface_t* surface_create(int w,int h)
{
	surface_t* surf = (surface_t*)kmalloc(sizeof(surface_t));
	surf->x = 0;
	surf->y = 0;
	surf->w = w<1?1:w;
	surf->h = h<1?1:h;
	surf->u8pp = screen_u8pp;
	surf->u8pl = surf->w * surf->u8pp;
	surf->buffer_size = surf->h * surf->u8pl;
	surf->buffer = (uint8_t*)kamalloc(sizeof(uint8_t)*surf->buffer_size);
	return surf;
}

void surface_free(surface_t* surface)
{
	//kfree(surface->buffer);
	kfree(surface);
}