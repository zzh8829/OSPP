#ifndef _VESA_H_
#define _VESA_H_ 

#include "system.h"

typedef struct 
{
	uint16_t attributes;
	uint8_t  winA, winB;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t segmentA, segmentB;
	uint32_t realFctPtr;
	uint16_t pitch;

	uint16_t Xres, Yres;
	uint8_t  Wchar, Ychar, planes, bpp, banks;
	uint8_t  memory_model, bank_size, image_pages;
	uint8_t  reserved0;

	uint8_t  red_mask, red_position;
	uint8_t  green_mask, green_position;
	uint8_t  blue_mask, blue_position;
	uint8_t  rsv_mask, rsv_position;
	uint8_t  directcolor_attributes;

	uint32_t physbase;
	uint32_t reserved1;
	uint16_t reserved2;
} __attribute__ ((packed)) vbe_info_t;

typedef struct 
{
	uint8_t VESASignature[4];
	int16_t VESAVersion;
	int8_t* OEMStringPtr;
	int32_t Capabilities;
	uint32_t *VideoModePtr;
	int16_t TotalMemory;
	int8_t reserved[236];
} __attribute__ ((packed)) vbe_block_t;


#endif
