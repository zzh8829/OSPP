#ifndef _USB_H_
#define _USB_H_
 
#include "system.h"

#define USB_STRING_SIZE			127

#define USB_FULL_SPEED			0x00
#define USB_LOW_SPEED			0x01
#define USB_HIGH_SPEED			0x02

// USB Request Type
#define RT_TRANSFER_MASK			0x80
#define RT_DEV_TO_HOST				0x80
#define RT_HOST_TO_DEV				0x00

#define RT_TYPE_MASK				0x60
#define RT_STANDARD					0x00
#define RT_CLASS					0x20
#define RT_VENDOR					0x40

#define RT_RECIPIENT_MASK			0x1f
#define RT_DEV						0x00
#define RT_INTF						0x01
#define RT_ENDP						0x02
#define RT_OTHER					0x03

//USB Device Request
#define REQ_GET_STATUS				0x00
#define REQ_CLEAR_FEATURE			0x01
#define REQ_SET_FEATURE				0x03
#define REQ_SET_ADDR				0x05
#define REQ_GET_DESC				0x06
#define REQ_SET_DESC				0x07
#define REQ_GET_CONF				0x08
#define REQ_SET_CONF				0x09
#define REQ_GET_INTF				0x0a
#define REQ_SET_INTF				0x0b
#define REQ_SYNC_FRAME				0x0c

// Descriptors

typedef struct
{
	uint8_t len;
	uint8_t type;
	uint8_t addr;
	uint8_t attributes;
	uint16_t maxPacketSize;
	uint8_t interval;
} __attribute__ ((packed)) usb_end_point_desc_t;

typedef struct
{
	uint8_t len;
	uint8_t type;
	uint8_t intfIndex;
	uint8_t altSetting;
	uint8_t endpCount;
	uint8_t intfClass;
	uint8_t intfSubClass;
	uint8_t intfProtocol;
	uint8_t intfStr;
} __attribute__ ((packed)) usb_interface_desc_t;

typedef struct
{
	uint8_t len;
	uint8_t type;
	uint16_t usbVer;
	uint8_t devClass;
	uint8_t devSubClass;
	uint8_t devProtocol;
	uint8_t maxPacketSize;
	uint16_t vendorId;
	uint16_t productId;
	uint16_t deviceVer;
	uint8_t vendorStr;
	uint8_t productStr;
	uint8_t serialStr;
	uint8_t confCount;
} __attribute__ ((packed)) usb_device_desc_t;

// Request

typedef struct usb_request_t
{
	uint8_t type;
	uint8_t req;
	uint16_t value;
	uint16_t index;
	uint16_t len;
} __attribute__ ((packed)) usb_request_t;


typedef struct
{
	usb_end_point_desc_t desc;
	uint32_t toggle;
} usb_end_point_t;

typedef struct
{
	usb_end_point_t *endp;
	usb_request_t *req;
	void *data;
	uint32_t len;
	bool complete;
	bool success;
} usb_transfer_t;

typedef struct _usb_device
{
	struct _usb_device *parent;
	struct _usb_device *next;

	void *hc;
	void *driver;

	uint32_t port;
	uint32_t speed;
	uint32_t addr;
	uint32_t maxPacketSize;

	usb_end_point_t endPoint;

	usb_interface_desc_t intfDesc;

	void (*hcControl)(struct _usb_device *dev, usb_transfer_t *t);
	void (*hcIntr)(struct _usb_device *dev, usb_transfer_t *t);
	void (*driverPoll)(struct _usb_device *dev);

} usb_device_t;

extern usb_device_t* g_usb_device;

typedef struct _usb_controller
{
	struct _usb_controller* next;
	void *hc;

	void (*poll)(struct _usb_controller *controller);
} usb_controller_t;
 
extern usb_controller_t* g_usb_controller;

usb_device_t* usb_device_create();
int usb_device_init(usb_device_t* dev);

/*


UsbDevice *UsbDevCreate();
bool UsbDevInit(UsbDevice *dev);
bool usb_request_tuest(UsbDevice *dev,
	uint32_ttype, uint32_trequest,
	uint32_tvalue, uint32_tindex,
	uint32_tlen, void *data);
bool UsbDevGetLangs(UsbDevice *dev, uint16_t *langs);
bool UsbDevGetString(UsbDevice *dev, char *str, uint32_tlangId, uint32_tstrIndex);
bool UsbDevClearHalt(UsbDevice *dev);
*/


#endif