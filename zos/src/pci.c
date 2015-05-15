#include "pci.h"
#include "pciids.h"
#include "kstdio.h"
#include "heap.h"

#include "ehci.h"

#define headerAddress(bus,dev,func,reg) 							\
	((uint32_t)((((bus) & 0xFF) << 16) | (((dev) & 0x1F) << 11) |	\
	(((func) & 0x07) << 8) | (((reg) & 0x3F) << 2) | 0x80000000L))	\


uint8_t pci_read_8(int bus, int dev, int function, int reg)
{
	uint32_t address = headerAddress(bus, dev, function, reg/4 );
	outl(PCI_CONFIG_PORT,address);
	return inb(PCI_DATA_PORT + (reg%4));
}

uint16_t pci_read_16(int bus, int dev, int function, int reg)
{
	uint32_t address = headerAddress(bus, dev, function, reg/2 );
	outl(PCI_CONFIG_PORT,address);
	return inw(PCI_DATA_PORT + (reg%2));
}

uint32_t pci_read_32(int bus, int dev, int function, int reg)
{
	uint32_t address = headerAddress(bus, dev, function, reg );
	outl(PCI_CONFIG_PORT,address);
	return inl(PCI_DATA_PORT);
}

void pci_write_8(int bus, int dev, int function, int reg, uint8_t data)
{
	uint32_t address = headerAddress(bus, dev, function, reg/4 );
	outl(PCI_CONFIG_PORT,address);
	return outb(PCI_DATA_PORT + (reg%4), data);
}

void pci_write_16(int bus, int dev, int function, int reg, uint16_t data)
{
	uint32_t address = headerAddress(bus, dev, function, reg/2 );
	outl(PCI_CONFIG_PORT,address);
	return outw(PCI_DATA_PORT + (reg%2), data);
}

void pci_write_32(int bus, int dev, int function, int reg, uint32_t data)
{
	uint32_t address = headerAddress(bus, dev, function, reg );
	outl(PCI_CONFIG_PORT,address);
	return outl(PCI_DATA_PORT, data);
}

void pci_read_device(int bus, int dev, int function, PCI_device_t* device)
{
	uint32_t address;
	size_t cnt;
	for(cnt=0;cnt<64;cnt++)
	{
		address = headerAddress(bus,dev,function,cnt);
		outl(PCI_CONFIG_PORT,address);
		device->header[cnt] = inl(PCI_DATA_PORT);
	}
}

void pci_device_dump(PCI_device_t* device)
{
	for(int i=0;i!=PCI_DEVICE_INFO_TABLE_LEN;i++)
	{
		PCI_device_info_t* info = &PCI_device_info_table[i];
		if(info->vendor_id == device->vendorId &&
			info->device_id == device->deviceId)
		{
			for(int i=0;i!=PCI_VENDOR_INFO_TABLE_LEN;i++)
			{
				if(PCI_vendor_info_table[i].vendor_id == device->vendorId)
				{
					kp("\t%2x:%2x %s %s",device->classCode,device->subClassCode,PCI_vendor_info_table[i].vendor_name,info->device_name);
				}
			}
			break;			
		}
	}
}

void pci_read_bar(uint32_t id, uint32_t idx,uint32_t* address,uint32_t* mask)
{

	uint32_t reg = 4 + idx;
	*address = pci_read_32(PCI_EXTRACT(id),reg);
	pci_write_32(PCI_EXTRACT(id),reg,0xffffffff);
	*mask = pci_read_32(PCI_EXTRACT(id),reg);
	pci_write_32(PCI_EXTRACT(id),reg,*address);
}

void pci_init()
{
	kp("PCI Initializing");

	PCI_device_t* device = (PCI_device_t*)kmalloc(sizeof(PCI_device_t));
	int cnt = 0;

	for(int bus = 0;bus!=0xFF;bus++)
	{
		for(int dev = 0;dev!=32;dev++)
		{
			for(int func = 0;func!=8;func++)
			{
				device->header[0] = pci_read_32(bus,dev,func,0);
				if(device->vendorId == 0 || device->vendorId == 0xFFFF || device->deviceId == 0xFFFF)
					continue;
				pci_read_device(bus,dev,func,device);
				//pci_device_dump(device);

				//ehci_init(PCI_ID(bus,dev,func),device);
				
				cnt++;
			}
		}
	}
}



