#include "usb.h"
#include "heap.h"
#include "timer.h"
#include "kstdio.h"

// USB Base Descriptor Types
#define USB_DESC_DEVICE                 0x01
#define USB_DESC_CONF                   0x02
#define USB_DESC_STRING                 0x03
#define USB_DESC_INTF                   0x04
#define USB_DESC_ENDP                   0x05

// USB HID Descriptor Types
#define USB_DESC_HID                    0x21
#define USB_DESC_REPORT                 0x22
#define USB_DESC_PHYSICAL               0x23

// USB HUB Descriptor Types
#define USB_DESC_HUB                    0x29

usb_device_t* g_usb_device;
usb_controller_t* g_usb_controller;

static int s_next_usb_addr;

usb_device_t* usb_device_create()
{
	usb_device_t* dev = (usb_device_t*)kmalloc(sizeof(usb_device_t));

	dev->parent = 0;
	//dev->next = g_usb_device;
	dev->next = 0;
	dev->hc = 0;
	dev->driver = 0;

	dev->port = 0;
	dev->speed = 0;
	dev->addr = 0;
	dev->maxPacketSize = 0;
	dev->endPoint.toggle = 0;

	dev->hcControl = 0;
	dev->hcIntr = 0;
	dev->driverPoll = 0;

	if(g_usb_device)
	{
		usb_device_t* next_dev = g_usb_device;
		while(next_dev->next) next_dev = next_dev->next;

		next_dev->next = dev;
	} else {
		g_usb_device = dev;
	}

	return dev;
}

int usb_device_request(usb_device_t* dev, uint32_t type, uint32_t req,
	uint32_t val, uint32_t idx, uint32_t len, void* data)
{
	usb_request_t request;
	request.type = type;
	request.req = req;
	request.value = val;
	request.index = idx;
	request.len = len;

	usb_transfer_t t;
	t.endp = 0;
	t.req = &request;
	t.data = data;
	t.len = len;
	t.complete = false;
	t.success = false;

	dev->hcControl(dev, &t);

	return t.success;
}

void usb_device_dump(usb_device_desc_t* desc)
{
	kp("vendor: %4x product: %4x",desc->vendorId,desc->productId);
}

int usb_device_init(usb_device_t* dev)
{
	usb_device_desc_t devDesc;

	usb_device_request(dev, RT_DEV_TO_HOST | RT_STANDARD| RT_DEV,
		REQ_GET_DESC, (USB_DESC_DEVICE << 8) | 0 ,0,
		8, &devDesc);

	kp("class: %2x:%2x",devDesc.devClass,devDesc.devSubClass);

	dev->maxPacketSize = devDesc.maxPacketSize;
	uint32_t addr = ++s_next_usb_addr;

	usb_device_request(dev, RT_HOST_TO_DEV | RT_STANDARD| RT_DEV,
		REQ_SET_ADDR, addr, 0,
		0,0);

	dev->addr = addr;
	timer_wait(2);

	usb_device_request(dev, RT_DEV_TO_HOST | RT_STANDARD| RT_DEV,
		REQ_GET_DESC, (USB_DESC_DEVICE << 8) |0 ,0,
		sizeof(usb_device_desc_t), &devDesc);

	usb_device_dump(&devDesc);

	return 1;
}