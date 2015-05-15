#ifndef _PCI_H_
#define _PCI_H_
 
#include "system.h"

#define PCI_CONFIG_PORT 	0x0CF8
#define PCI_DATA_PORT 		0x0CFC

#define PCI_ID(bus,dev,func) ( (((bus) & 0xFF) << 16) | \
	(((dev) & 0x1F) << 11) | (((func) & 0x07) << 8) )
#define PCI_BUS(id)	((id >> 16)&0xFF)
#define PCI_DEV(id) ((id >> 11)&0x1F)
#define PCI_FUNC(id) ((id >> 8)&0x07)
#define PCI_EXTRACT(id) PCI_BUS(id),PCI_DEV(id),PCI_FUNC(id)

typedef union 
{
	struct 
	{
		uint16_t vendorId;
		uint16_t deviceId;
		uint16_t commandReg;
		uint16_t statusReg;
		uint8_t revisionId;
		uint8_t progIF;
		uint8_t subClassCode;
		uint8_t classCode;
		uint8_t cachelineSize;
		uint8_t latency;
		uint8_t headerType;
		uint8_t BIST;
		
		union 
		{
			struct 
			{
				uint32_t baseAddress[6];
				uint32_t cardBusCIS;
				uint16_t subsystemVendorId;
				uint16_t subsystemDeviceId;
				uint32_t expansionROM;
				uint8_t capPtr;
				uint8_t reserved1[3];
				uint32_t reserved2[1];
				uint8_t interruptLine;
				uint8_t interruptPin;
				uint8_t minGrant;
				uint8_t maxLatency;
				uint32_t deviceSpecific[48];
			};
			struct 
			{
				uint32_t baseAddress[2];
				uint8_t primaryBus;
				uint8_t secondaryBus;
				uint8_t subordinateBus;
				uint8_t secondaryLatency;
				uint8_t ioBaseLow;
				uint8_t ioLimitLow;
				uint16_t secondaryStatus;
				uint16_t memoryBaseLow;
				uint16_t memoryLimitLow;
				uint16_t prefetchBaseLow;
				uint16_t prefetchLimitLow;
				uint32_t prefetchBaseHigh;
				uint32_t prefetchLimitHigh;
				uint16_t ioBaseHigh;
				uint16_t ioLimitHigh;
				uint32_t reserved2[1];
				uint32_t expansionROM;
				uint8_t interruptLine;
				uint8_t interruptPin;
				uint16_t bridgeControl;
				uint32_t deviceSpecific[48];
			} bridge;
			struct 
			{
				uint32_t exCaBase;
				uint8_t capPtr;
				uint8_t reserved05;
				uint16_t secondaryStatus;
				uint8_t pciBus;
				uint8_t bardBusBus;
				uint8_t subordinateBus;
				uint8_t latencyTimer;
				uint32_t memoryBase0;
				uint32_t memoryLimit0;
				uint32_t memoryBase1;
				uint32_t memoryLimit1;
				uint16_t ioBase0Low;
				uint16_t ioBase0High;
				uint16_t ioLimit0Low;
				uint16_t ioLimit0High;
				uint16_t ioBase1Low;
				uint16_t ioBase1High;
				uint16_t ioLimit1Low;
				uint16_t ioLimit1High;
				uint8_t interruptLine;
				uint8_t interruptPin;
				uint16_t bridgeControl;
				uint16_t subsystemVendorId;
				uint16_t subsystemDeviceId;
				uint32_t legacyBaseAddr;
				uint32_t cardbusReserved[14];
				uint32_t vendorSpecific[32];
			} cardBus;
		};
	};

	uint32_t header[64];

} __attribute__((packed)) PCI_device_t;


void pci_init();
void pci_read_bar(uint32_t id, uint32_t idx,uint32_t* address,uint32_t* mask);
uint8_t pci_read_8(int bus, int dev, int function, int reg);
uint16_t pci_read_16(int bus, int dev, int function, int reg);
uint32_t pci_read_32(int bus, int dev, int function, int reg);
void pci_write_8(int bus, int dev, int function, int reg, uint8_t data);
void pci_write_16(int bus, int dev, int function, int reg, uint16_t data);
void pci_write_32(int bus, int dev, int function, int reg, uint32_t data);

#endif