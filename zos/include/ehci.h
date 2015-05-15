#ifndef _EHCI_H_
#define _EHCI_H_

#include "system.h"
#include "pci.h"

void ehci_init(uint32_t id, PCI_device_t* info);
 
#endif