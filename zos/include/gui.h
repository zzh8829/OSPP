#ifndef _GUI_H_
#define _GUI_H_
 
#include "system.h"
#include "graphics.h"
#include "list.h"

typedef enum
{
	GE_OPEN,
	GE_CLOSE,
	GE_MOUSE_DOWN,
	GE_MOUSE_UP,
	GE_KEY_DOWN,
	GE_KEY_UP,
} GUI_EVENT;

struct _window;

typedef struct
{
	void (*render)(struct _window* win);
	void (*update)(struct _window* win);
} window_func;

typedef struct _window
{	
	union
	{
		struct
		{
			int x,y,w,h;
		};
		rect_t rect;
	};

	window_func win_func;

	surface_t* surface;

	struct _window* parent;
	struct _window* subwindow_list;
	list_t list;
} window_t;

extern window_t g_window_list; 

window_t* window_create(window_t* parent,int x, int y,int w, int h);
void window_render(window_t* win);
void window_update(window_t* win);
void window_move(window_t* win,int dx, int dy );
void window_set_callback(window_t* win, GUI_EVENT eve,void* func);

void gui_init();
void gui_print();

void gui_update();
void gui_render();
 
#endif	