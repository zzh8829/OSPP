#include "system.h"
#include "multiboot.h"
#include "vesa.h"
#include "kstdio.h"
#include "gdt.h"
#include "idt.h"
#include "isrs.h"
#include "irq.h"
#include "timer.h"
#include "graphics.h"
#include "keyboard.h"
#include "elf.h"
#include "time.h"
#include "paging.h"
#include "heap.h"
#include "pci.h"
#include "gui.h"
#include "font.h"
#include "cpu.h"

// YY.MM.DD
void get_version(char* buf)
{
	char tmp[12];
	memcpy(tmp,__DATE__,12);
	int month=0;
	const char* abbr[13]={ "???",
		"Jan","Feb","Mar","Apr","May","Jun",
		"Jul","Aug","Sep","Oct","Nov","Dec",
	};
	for(;month<=13;month++)
		if(strncmp(tmp,abbr[month],3)==0)
			break;
	buf[0]=tmp[9];
	buf[1]=tmp[10];
	buf[2]='.';
	buf[3]=month?month/10+'0':'?';
	buf[4]=month?month%10+'0':'?';
	buf[5]='.';
	buf[6]=tmp[4]==' '?'0':tmp[4];
	buf[7]=tmp[5];
	buf[8]='\0';
}

int check_flag(int flags, int bit)
{
	return flags & (1<<bit);
}

char gfx_con_buf[25][80];
int gfx_con_x = 0;
int gfx_con_y = 0;

void gfx_print(char ch)
{
	if(ch==27)
	{
		reboot();
	}
	else if(ch=='\n')
	{
		gfx_con_y++;
		gfx_con_x = 0;
	}
	else if(ch=='\t')
	{
		int x = gfx_con_x/4*4 + 4;
		if(gfx_con_x < 76)
		{
			for(int i=gfx_con_x;i<=x;i++)
			{
				gfx_con_buf[gfx_con_y][i]=' ';
			}
			gfx_con_x=x;
		} else {
			gfx_con_y++;
			gfx_con_x=0;
		}	
	} else if(ch=='\b')
	{
		if(gfx_con_x == 0)
		{
			if(gfx_con_y!=0)
			{
				gfx_con_buf[gfx_con_y][gfx_con_x] = ' ';
				gfx_con_y--;
				gfx_con_x = 79;
			}
		} else {
			gfx_con_x--;
			gfx_con_buf[gfx_con_y][gfx_con_x] = ' ';
		}
	}
	else if(ch>=32 &&
		ch<=126)
	{
		gfx_con_buf[gfx_con_y][gfx_con_x] = ch;
		gfx_con_x++;
		if(gfx_con_x==80)
		{
			gfx_con_x = 0;
			gfx_con_y ++;
		}
	}
	if(gfx_con_y>24)
	{
		for(int i=0;i!=25;i++)
		{
			memset(gfx_con_buf[i],0,sizeof(char)*80);
		}
		gfx_con_y = 0;
	}
}

void kernel_main(uint32_t mb_addr, uint32_t mb_magic)
{
	if(mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		kprintf(PANIC,"Wrong Bootloader Magic\n");
		for(;;);
	}

	multiboot_info_t* mb_info = (multiboot_info_t*)mb_addr;
	vbe_info_t* vbe_info = (vbe_info_t *)(mb_info->vbe_mode_info);

	uint32_t top = mb_info->mem_upper;
	uint32_t bot = mb_info->mem_lower;
	uint32_t mem = top+bot;

	clear_console();

	char version[20];
	get_version(version);
	kp("Welcome to ZOS v%s",version);

	kp("System Information");
	kp("	Booting Flag: 0x%x",mb_info->flags);
	if(check_flag(mb_info->flags,0))
		kp("	Available Memory: %d KB / %d MB",mem,mem/1024);
	if(check_flag(mb_info->flags,1))
		kp("	Booting Device: 0x%x",mb_info->boot_device);
	if(check_flag(mb_info->flags,2))
	{
		if(strlen((char*)mb_info->cmdline)>1)
			kp("	Booting Command: %s",(char*)mb_info->cmdline);
	}
	if(check_flag(mb_info->flags,3))
		{}
	if(check_flag(mb_info->flags,4))
		{}
	if(check_flag(mb_info->flags,5))
		kernel_elf = elf_from_multiboot(mb_info);
	if(check_flag(mb_info->flags,6))
	{
		/*
		multiboot_memory_map_t *mmap;
		kp ("mmap_addr = 0x%x, mmap_length = 0x%x",
			(unsigned) mb_info->mmap_addr, (unsigned) mb_info->mmap_length);
		for (mmap = (multiboot_memory_map_t *) mb_info->mmap_addr;
			(unsigned long) mmap < mb_info->mmap_addr + mb_info->mmap_length;
			mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
			+ mmap->size + sizeof (mmap->size)))
		{
			kp (" size = 0x%x, base_addr = 0x%x%x,"
			" length = 0x%x%x, type = 0x%x",
			(unsigned) mmap->size,
			mmap->addr >> 32,
			mmap->addr & 0xffffffff,
			mmap->len >> 32,
			mmap->len & 0xffffffff,
			(unsigned) mmap->type);
		}
		*/		
	}
	gdt_init();		/* Global Descriptor Table */
	idt_init();		/* Interrupt Descriptor Table */
	isrs_init();	/* Interrupt Service Requests */
	irq_init();		/* Hardware Interrupt Requests */ 

	open_serial_output();

	timer_init();
	paging_init(mem);
	heap_init();
	
	keyboard_init();

	//pci_init();
	kp("login: ");
	
	graphics_init(vbe_info);
	font_init();
	gui_init();
	
	int frame = 0;

	int tick_last = clock();

	uint32_t fpstick = clock();
	uint32_t fps = 60;	

	int w = 720;
	int h = 480;
	int x = (g_screen->w-w)/2;
	int y = (g_screen->h-h)/2;
	if(x<0){x=20;w = screen_width-40;}
	if(y<0){y=20;h = screen_height-40;}

	

	color_t col = {0x00000000};

	window_t* win = window_create(0,x,y,w,h);

	/*
	gfx_con_buf = (char**)kmalloc(sizeof(char*)*25);
	for(int i=0;i!=25;i++)
	{
		gfx_con_buf[i] = (char*)kzmalloc(sizeof(char)*80);
	}
	*/

	char* str[10]=  
	{
		"Welcome to ZOS\n",
		"Hello!\n",
		"abcdefghijklmnopqrstuvwxyz1234567890\n",
		0
	};
	for(int i=0;i!=3;i++)
	{
		for(int j = 0;j!=strlen(str[i]);j++)
		{
			gfx_print(str[i][j]);
		}
	}
	

	void render(window_t* win)
	{
	}

	void update(window_t* win)
	{
		surface_fill(win->surface,0,rgb(255,255,255));

		key_event_t* head;
		int len;
		keyboard_event_get(&head,&len);
		for(int i=0;i!=len;i++)
		{
			key_event_t* eve = head+i;
			if(eve->type == KEY_DOWN)
			{
				gfx_print(eve->character);
			}
		}
		keyboard_event_clear();
		
		rect_t trect = {0,0,720,20};
		for(int i=0;i!=24;i++)
		{
			if(strlen(gfx_con_buf[i]))
			{
				surface_t* surf = font_render(gfx_con_buf[i],col);
				surface_blend(surf,0,win->surface,&trect);
				surface_free(surf);
			}
			trect.y += 20;
		}
	}

	win->win_func.render = render;	
	win->win_func.update = update;

	struct tm timeinfo;
	char tbuf[30];
	surface_t* fpssurf = 0;

	for(ever)
	{
		uint32_t tick_beg = clock();

		surface_fill(g_screen,0,rgb(177,177,177));
		
		gui_update();
		gui_render();

		if(fpssurf)
		{
			rect_t fpsrect = {g_screen->w - fpssurf->w,
				g_screen->h - fpssurf->h,
				fpssurf->w, fpssurf->h }; 
			surface_blend(fpssurf,0,g_screen,&fpsrect);
		}
	

		flip();

		if(clock()-fpstick > TICKS_PER_SEC)
		{
			read_rtc_time(&timeinfo);
			sprintf(tbuf,"20%d-%d-%d %d:%d:%d",
			timeinfo.tm_year,timeinfo.tm_mon,timeinfo.tm_mday,
			timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);

			if(fpssurf)surface_free(fpssurf);
			fpssurf = font_render(tbuf,col);
		}
		if(clock()-tick_beg < TICKS_PER_SEC/fps)
		{
			timer_wait(TICKS_PER_SEC/fps-(clock()-tick_beg));
		}
		tick_last = clock();		
	}
//#endif
	for(ever);
}
