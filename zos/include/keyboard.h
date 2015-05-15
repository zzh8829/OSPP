#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "system.h"
#include "list.h"

#define KEY_DOWN 0
#define KEY_UP 1

#define KEY_PRESSED 1
#define KEY_RELEASED 0

typedef struct 
{
	uint8_t type;
	uint8_t state;
	uint8_t scancode;
	uint8_t character;
} key_event_t;;

typedef struct 
{
	key_event_t event;
	list_t list;

} key_event_list;

extern int keylock;

extern key_event_list g_key_event;

extern key_event_t key_event_buffer[10];
extern int key_event_pos;
extern int key_event_len;

void keyboard_init();
int keyboard_press(int code);

void keyboard_event_get(key_event_t** eve,int* len);
void keyboard_event_clear();
 
#endif