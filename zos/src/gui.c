#include "gui.h"
#include "graphics.h"
#include "heap.h"
#include "kstdio.h"

window_t g_window_list;

void gui_init()
{
	LIST_INIT(g_window_list.list);
}

void gui_background_render()
{
	//fillrect(0,0,screen_width,screen_height,rgb(255,255,255));
}


void gui_print()
{

}

void gui_update()
{
	window_t* win;
	LIST_FOR_EACH(win,g_window_list.list,list)
	{
		window_update(win);
	}
}

void gui_render()
{
	gui_background_render();
	window_t* win;
	LIST_FOR_EACH(win,g_window_list.list,list)
	{
		window_render(win);
	}
}

window_t* window_create(window_t* parent,int x,int y,int w, int h)
{
	window_t* win = (window_t*)kmalloc(sizeof(window_t));
	win->x = x;
	win->y = y;
	win->w = w;
	win->h = h;

	win->parent = parent;	
	
	win->surface = surface_create(w,h);
	win->subwindow_list = (window_t*)kmalloc(sizeof(window_t));

	LIST_INIT(win->subwindow_list->list);
	LIST_PUSH(g_window_list.list,win->list);

	return win;
}

void window_update(window_t* win)
{
	if(win->win_func.update)
	{
		win->win_func.update(win);
	}
}

void window_render(window_t* win)
{
	if(win->win_func.render)
	{
		win->win_func.render(win);
	}

	if(!win->parent)
	{
		surface_blit(win->surface,0,
			g_screen,&win->rect);
	} else {
		surface_blit(win->surface,0,
			win->parent->surface,&win->rect);
	}
	
}