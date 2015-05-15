#include <stdint.h>

struct VBEControllerInfo
{
	char signature[4];
	uint16_t version;
	uint16_t oemString[2];
	uint8_t capabilities[4];
	uint16_t videomodes[2];
	uint16_t maxMemory;
};

struct VBEModeInfo
{
	uint16_t attributes;
	uint8_t winA,winB;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t segmentA,segmentB;
	uint16_t realFctPtr[2];
	uint16_t pitch;
	uint16_t Xres,Yres;
	uint8_t Wchar,Ychar,planes, bpp ,banks;
	uint8_t memory_model, bank_size, image_pages;
	uint8_t reserved0;
	uint8_t red_mask,red_position;
	uint8_t green_mask,green_position;
	uint8_t blue_mask,blue_position;
	uint8_t rsv_mask, rsv_position;
	uint8_t directcolor_attributes;
	uint32_t physbase;
	uint32_t reserved1;
	uint16_t reserved2;
};

struct VBEModeInfo* vbeModeInfo; 
struct VBEControllerInfo* vbeControllerInfo;
uint8_t* vbuf;

void init_graphics(uint8_t* _vbuf);
void set_pixel(int x,int y,int color);