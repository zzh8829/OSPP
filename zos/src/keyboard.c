#include "keyboard.h"
#include "irq.h"
#include "kstdio.h"
#include "heap.h"

typedef void (*key_func_t)(void);

static uint8_t shift_down = 0;
static uint8_t ctrl_down = 0;
static uint8_t alt_down = 0;
static uint8_t key_code;

char kbuf[256];
int kbufp  = 0;

int keylock = 0;

uint8_t keystates[256];

static const char key_map[0x3a][2] = {
	/*00*/{0x0, 0x0}, { 27, 27 }, {'1', '!'}, {'2', '@'}, 
	/*04*/{'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'}, 
	/*08*/{'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'},
	/*0c*/{'-', '_'}, {'=', '+'}, {'\b','\b'},{'\t','\t'},
	/*10*/{'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'},
	/*14*/{'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'},
	/*18*/{'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'},
	/*1c*/{'\n','\n'},{0x0, 0x0}, {'a', 'A'}, {'s', 'S'},
	/*20*/{'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'},
	/*24*/{'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
	/*28*/{'\'','\"'},{'`', '~'}, {0x0, 0x0}, {'\\','|'}, 
	/*2c*/{'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'}, 
	/*30*/{'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'},
	/*34*/{'.', '>'}, {'/', '?'}, {0x0, 0x0}, {'*', '*'},
	/*38*/{0x0, 0x0}, {' ', ' '} 
};

static void pln()
{
	if( key_code & 0x80)
		return;
	kbufp&=0xff;
	kbuf[kbufp] = key_map[key_code&0x7f][shift_down];
	kprintf(DUMP,"%c",kbuf[kbufp],WHITE,BLACK);
	kbufp++;
}

/* Ctrl */
static void ctl(void)
{
	ctrl_down ^= 0x1;
}
 
/* Alt */
static void alt(void)
{
	alt_down ^= 0x1;
}
 
/* Shift */
static void shf(void)
{
	shift_down ^= 0x1;
}
 
/* F1, F2 ~ F12 */
static void fun(void)
{
}
 
/* not implementated */
static void unp(void)
{
}

key_func_t key_funcs[128] = {
	/*00*/unp, unp, pln, pln, pln, pln, pln, pln,
	/*08*/pln, pln, pln, pln, pln, pln, pln, pln,
	/*10*/pln, pln, pln, pln, pln, pln, pln, pln,
	/*18*/pln, pln, pln, pln, pln, ctl, pln, pln,
	/*20*/pln, pln, pln, pln, pln, pln, pln, pln,
	/*28*/pln, pln, shf, pln, pln, pln, pln, pln,
	/*30*/pln, pln, pln, pln, pln, pln, shf, pln,
	/*38*/alt, pln, unp, fun, fun, fun, fun, fun,
	/*40*/fun, fun, fun, fun, fun, unp, unp, unp,
	/*48*/unp, unp, unp, unp, unp, unp, unp, unp,
	/*50*/unp, unp, unp, unp, unp, unp, unp, fun,
	/*58*/fun, unp, unp, unp, unp, unp, unp, unp,
	/*60*/unp, unp, unp, unp, unp, unp, unp, unp,
	/*68*/unp, unp, unp, unp, unp, unp, unp, unp,
	/*70*/unp, unp, unp, unp, unp, unp, unp, unp,
	/*78*/unp, unp, unp, unp, unp, unp, unp, unp,
};

key_event_t key_event_buffer[10];
int key_event_pos = 0;
int key_event_len = 10;

void keyboard_event_get(key_event_t** eve,int* len)
{
	*eve = key_event_buffer;
	*len = key_event_pos;
}

void keyboard_event_clear()
{
	key_event_pos = 0;
}

static void keyboard_handler(struct regs* r)
{
	key_code = inb(0x60);
	keystates[key_code &0x7f] ^= 1 ;

/*
	key_event_list* eve = (key_event_list*)kmalloc(sizeof(key_event_list));
	eve->event.scancode = key_code;
	eve->event.state = keystates[key_code &0x7f];
	eve->event.character = key_map[key_code&0x7f][shift_down];
	eve->event.type = key_code &0x80 ? KEY_UP: KEY_DOWN;
	LIST_PUSH(g_key_event.list,eve->list);
*/
	if(key_event_pos < key_event_len)
	{
		key_event_t* eve = &key_event_buffer[key_event_pos];
		eve->scancode = key_code;
		eve->state = keystates[key_code &0x7f];
		eve->character = key_map[key_code&0x7f][shift_down];
		eve->type = key_code &0x80 ? KEY_UP: KEY_DOWN;

		key_event_pos++;
	}



	(*key_funcs[key_code&0x7f])();
	int com = inb(0x61);
	outb(0x61,com|0x80);
   	outb(0x61,com&0x7f);
}

key_event_list g_key_event;

void keyboard_init()
{
	//LIST_INIT(g_key_event.list);
	irq_install_handler(1,keyboard_handler);
}