#include "ehci.h"
#include "kstdio.h"
#include "paging.h"
#include "heap.h"
#include "timer.h"
#include "usb.h"

#define USBLEGSUP					0x00		// USB Legacy Support Extended Capability
#define USBLEGCTLSTS				0x04		// USB Legacy Support Control/Status

#define USBLEGSUP_HC_OS				0x01000000	// HC OS Owned Semaphore
#define USBLEGSUP_HC_BIOS			0x00010000	// HC BIOS Owned Semaphore
#define USBLEGSUP_EECP_MASK			0x0000ff00	// Next EHCI Extended Capability Pointer
#define USBLEGSUP_CAPID				0x000000ff	// Capability ID

#define USB_PACKET_OUT				0			// token 0xe1
#define USB_PACKET_IN				1			// token 0x69
#define USB_PACKET_SETUP			2			// token 0x2d

typedef struct 
{
	volatile uint8_t capLength;
	volatile uint8_t reserved;
	volatile uint16_t hciVersion;

	volatile union
	{
		volatile uint32_t hcsParams; // Host Comtroller Structural Parameters
#define HCSP_N_PORTS_MASK			(15 << 0)	// Number of Ports
#define HCSP_PPC					(1 << 4)	// Port Power Control
#define HCSP_PORT_ROUTE				(1 << 7)	// Port Routing Rules
#define HCSP_N_PCC_MASK				(15 << 8)	// Number of Ports per Companion Controller
#define HCSP_N_PCC_SHIFT			(8)	
#define HCSP_N_CC_MASK				(15 << 12)	// Number of Companion Controllers
#define HCSP_N_CC_SHIFT				(12)	
#define HCSP_P_INDICATOR			(1 << 16)	// Port Indicator
#define HCSP_DPN_MASK				(15 << 20)	// Debug Port Number
#define HCSP_DPN_SHIFT				(20)
		volatile struct 
		{
			volatile uint32_t numPort: 4;
			volatile uint32_t PPC: 1;
			volatile uint32_t reserved1: 2;
			volatile uint32_t portRoute: 1;
			volatile uint32_t numPCC: 4;
			volatile uint32_t numCC: 4;
			volatile uint32_t portIndicator: 1;
			volatile uint32_t reserved2: 3;
			volatile uint32_t DPN: 4;
			volatile uint32_t reserved3: 8;
		} __attribute__ ((packed)) hcsp;	
	};

	volatile union
	{
		volatile uint32_t hccParams;	// Host Controller Capability Parameters
#define HCCP_64_BIT					(1 << 0)	// 64-bit Addressing Capability
#define HCCP_PFLF					(1 << 1)	// Programmable Frame List Flag
#define HCCP_ASPC					(1 << 2)	// Asynchronous Schedule Park Capability
#define HCCP_IST_MASK				(15 << 4)	// Isochronous Sheduling Threshold
#define HCCP_EECP_MASK				(255 << 8)	// EHCI Extended Capabilities Pointer
#define HCCP_EECP_SHIFT				8
		volatile struct 
		{
			volatile uint32_t bit64: 1;
			volatile uint32_t PFLF: 1;
			volatile uint32_t ASPC: 1;
			volatile uint32_t reserved1: 1;
			volatile uint32_t IST: 4;
			volatile uint32_t EECP: 8;
			volatile uint32_t reserved2: 16;
		} __attribute__ ((packed)) hccp;
	};
	volatile uint64_t portRoute;
}__attribute__ ((packed)) ehci_cap_regs_t;

typedef struct 
{
	volatile uint32_t command;
#define CMD_RUN						(1<<0)		/* start/stop HC */
#define CMD_RESET					(1<<1)		/* reset HC not bus */
#define CMD_FLS_MASK				(3 << 2)	// Frame List Size
#define CMD_FLS_SHIFT				(2)
#define CMD_PSE						(1<<4)		/* periodic schedule enable */
#define CMD_ASE						(1<<5)		/* async schedule enable */
#define CMD_IOAA					(1<<6)		/* "doorbell" interrupt on async advance */
#define CMD_LRESET					(1<<7)		/* partial reset (no ports, etc) */
#define CMD_ASPMC_MASK				(3<<8)		// Asynchronous Schedule Park Mode
#define CMD_ASPMC_SHIFT				(8)
#define CMD_ASPM					(1<<11)
#define CMD_ITC_MASK				(255<<16)	// Interrupt Threshold Control
#define CMD_ITC_SHIFT				(16)
#define CMD_PARK_CNT(c)				(((c)>>8)&3)	/* how many transfers to park for */

	volatile uint32_t status;
#define STS_INT						(1 << 0)	// USB Interrupt
#define STS_ERR						(1 << 1)	// USB Error Interrupt
#define STS_PCD						(1 << 2)	// Port Change Detect
#define STS_FLR						(1 << 3)	// Frame List Rollover
#define STS_HSE						(1 << 4)	// Host System Error
#define STS_IOAA					(1 << 5)	// Interrupt on Async Advance
#define STS_HALT					(1 << 12)	// Host Controller Halted
#define STS_RECL					(1 << 13)	// Reclamation
#define STS_PSS						(1 << 14)	// Periodic Schedule Status
#define STS_ASS						(1 << 15)	// Asynchronous Schedule Status

	volatile uint32_t usbIntr;
	volatile uint32_t frameIndex;
	volatile uint32_t segment;
	volatile uint32_t frameList;
	volatile uint32_t asyncListAddr;
	volatile uint32_t reserved[9];
	volatile uint32_t configFlag;
	volatile uint32_t ports[];
#define PORT_CONNECT				(1 << 0)// Current Connect Status
#define PORT_CSC					(1 << 1)// Connect Status Change
#define PORT_ENABLE					(1 << 2)// Port Enabled
#define PORT_ENABLE_CHANGE			(1 << 3)// Port Enable Change
#define PORT_OC						(1 << 4)// Over-current Active
#define PORT_OC_CHANGE				(1 << 5)// Over-current Change
#define PORT_FPR					(1 << 6)// Force Port Resume
#define PORT_SUSPEND				(1 << 7)// Suspend
#define PORT_RESET					(1 << 8)// Port Reset
#define PORT_LS_MASK				(3 << 10) // Line Status
#define PORT_LS_SHIFT				(10)
#define PORT_POWER					(1 << 12) // Port Power
#define PORT_OWNER					(1 << 13) // Port Owner
#define PORT_IC_MASK				(3 << 14) // Port Indicator Control
#define PORT_IC_SHIFT				(14)
#define PORT_TC_MASK				(15 << 16)// Port Test Control
#define PORT_TC_SHIFT				(16)
#define PORT_WKOCONN_E				(1 << 20) // Wake on Connect Enable
#define PORT_WKODISC_E				(1 << 21) // Wake on Disconnect Enable
#define PORT_WKOC_E					(1 << 22) // Wake on Over-current Enable
#define PORT_RWC					(PORT_CSC | PORT_ENABLE_CHANGE | PORT_OC_CHANGE)

}__attribute__ ((packed)) ehci_op_regs_t;

typedef struct _ehci_qtd
{
	volatile uint32_t nextPtr;
	volatile uint32_t alternatePtr;
	volatile uint32_t token;
#define QTD_PING					(1 << 0)		// Ping State
#define QTD_STS						(1 << 1)		// Split Transaction State
#define QTD_MMF						(1 << 2)		// Missed Micro-Frame
#define QTD_XACT					(1 << 3)		// Transaction Error
#define QTD_BABBLE					(1 << 4)		// Babble Detected
#define QTD_DBE						(1 << 5)		// Data Buffer Error
#define QTD_HALT					(1 << 6)		// Halted
#define QTD_ACTIVE					(1 << 7)		// Active
#define QTD_PID_MASK				(3 << 8)		// PID Code
#define QTD_PID_SHIFT				(8)
#define QTD_PID(tok)				(((tok) & QTD_PID_MASK)>> QTD_PID_SHIFT)
#define QTD_CERR_MASK				(3 << 10)		// Error Counter
#define QTD_CERR_SHIFT				(10)
#define QTD_CERR(tok)				(((tok) & QTD_CERR_MASK) >> QTD_CERR_SHIFT)
#define QTD_PAGE_MASK				(7 << 12)		// Current Page
#define QTD_PAGE_SHIFT				(12)
#define QTD_IOC						(1 << 15)		// Interrupt on Complete
#define QTD_LEN_MASK				(0x7fff0000)	// Total Bytes to Transfer
#define QTD_LEN_SHIFT				(16)
#define QTD_TOG						(1 << 31)		// Data Toggle
#define QTD_TOG_SHIFT				(31)

	volatile uint32_t buffer[5];
	volatile uint32_t extBuffer[5];
	
	uint32_t physical;
	uint32_t used;
	struct _ehci_qtd* next;

} ehci_qtd_t;

#define Q_TYPE_END					(1 << 0)
#define Q_TYPE_ITD					(0 << 1)
#define Q_TYPE_QH					(1 << 1)
#define Q_TYPE_SITD					(2 << 1)
#define Q_TYPE_FSTN					(3 << 1)

typedef struct _ehci_qh
{
	volatile uint32_t qhhlp; // Queue Head Horizontal Link Pointer
	volatile uint32_t epch;// Endpoint Characteristics
#define QH_CH_ADDR_MASK				0x0000007f	// Device Address
#define QH_CH_INACTIVE				0x00000080	// Inactive on Next Transaction
#define QH_CH_EPN_MASK				0x00000f00	// Endpoint Number
#define QH_CH_EPN_SHIFT				8
#define QH_CH_EPS_MASK				0x00003000	// Endpoint Speed
#define QH_CH_EPS_SHIFT				12
#define QH_CH_DTC					0x00004000	// Data Toggle Control
#define QH_CH_HEAD					0x00008000	// Head of Reclamation List Flag
#define QH_CH_MPL_MASK				0x07ff0000	// Maximum Packet Length
#define QH_CH_MPL_SHIFT				16
#define QH_CH_CONTROL				0x08000000	// Control Endpoint Flag
#define QH_CH_NAK_MASK				0xf0000000	// Nak Count Reload
#define QH_CH_NAK_SHIFT				28

	volatile uint32_t epcap; // Endpoint Capabilities
#define QH_CAP_SCHED_SHIFT			0
#define QH_CAP_SCHED_MASK			0x000000ff	// Interrupt Schedule Mask
#define QH_CAP_COMPL_SHIFT			8
#define QH_CAP_COMPL_MASK			0x0000ff00	// Split Completion Mask
#define QH_CAP_HUB_ADDR_SHIFT		16
#define QH_CAP_HUB_ADDR_MASK		0x007f0000	// Hub Address
#define QH_CAP_HUB_PORT_MASK		0x3f800000	// Port Number
#define QH_CAP_HUB_PORT_SHIFT		23
#define QH_CAP_MULT_MASK			0xc0000000	// High-Bandwidth Pipe Multiplier
#define QH_CAP_MULT_SHIFT			30

	volatile uint32_t currentPtr;

	volatile uint32_t nextPtr;
	volatile uint32_t alternatePtr;
	volatile uint32_t token;
	volatile uint32_t buffer[5];
	volatile uint32_t extBuffer[5];

	uint32_t used;
	uint32_t physical;

	ehci_qtd_t* qtdHead;

	struct _ehci_qh* prev;
	struct _ehci_qh* next;

	usb_transfer_t* transfer;

} __attribute__ ((aligned(128),packed)) ehci_qh_t;

typedef struct 
{
	volatile ehci_cap_regs_t *capRegs;
	volatile ehci_op_regs_t *opRegs;
	volatile uint32_t *frameList;
	ehci_qh_t* qhPool;
	ehci_qtd_t* qtdPool;
	ehci_qh_t* asyncQH;
	ehci_qh_t* periodicQH;
} ehci_controller_t;

static void dbg_command (ehci_controller_t* ehci)
{
	uint32_t command= ehci->opRegs->command;
	uint32_t async= ehci->opRegs->asyncListAddr;

	kp("async_next: %08x",async);
	kp(
	"command %06x %s=%d ithresh=%d%s%s%s%s %s %s",
	 command,
	(command & CMD_ASPM) ? "park" : "(park)",
	CMD_PARK_CNT (command),
	(command >> 16) & 0x3f,
	(command & CMD_LRESET) ? " LReset" : "",
	(command & CMD_IOAA) ? " IOAA" : "",
	(command & CMD_ASE) ? " Async" : "",
	(command & CMD_PSE) ? " Periodic" : "",
	(command & CMD_RESET) ? " Reset" : "",
	(command & CMD_RUN) ? "RUN" : "HALT"
	);
}
static void dbg_status (ehci_controller_t* ehci)
{
	uint32_t status= ehci->opRegs->status;
	kp(
	"status %04x%s%s%s%s%s%s%s%s%s%s",
	status,
	(status & STS_ASS) ? " Async" : "",
	(status & STS_PSS) ? " Periodic" : "",
	(status & STS_RECL) ? " Recl" : "",
	(status & STS_HALT) ? " Halt" : "",
	(status & STS_IOAA) ? " IOAA" : "",
	(status & STS_HSE) ? " FATAL" : "",
	(status & STS_FLR) ? " FLR" : "",
	(status & STS_PCD) ? " PCD" : "",
	(status & STS_ERR) ? " ERR" : "",
	(status & STS_INT) ? " INT" : ""
	);
}


ehci_qtd_t* ehci_alloc_qtd(ehci_controller_t* hc)
{
	ehci_qtd_t * end = hc->qtdPool + 8;
	for(ehci_qtd_t* qtd = hc->qtdPool; qtd!= end; qtd++)
	{
		if(!qtd->used)
		{
			qtd->used = 1;
			qtd->physical = physical_address(qtd);
			return (ehci_qtd_t*)qtd->physical;
		}
	}
	kp("qtd Alloc failed");
	return 0;
}

void ehci_free_qtd(ehci_qtd_t* qtd)
{
	qtd->used = 0;
}

ehci_qh_t* ehci_alloc_qh(ehci_controller_t* hc)
{
	ehci_qh_t * end = hc->qhPool + 8;
	for(ehci_qh_t* qh = hc->qhPool; qh!= end; qh++)
	{
		if(!qh->used)
		{
			qh->used = 1;
			qh->physical = physical_address(qh);
			return (ehci_qh_t*)qh->physical;
		}
	}
	kp("qh Alloc failed");
	return 0;
}

void ehci_free_qh(ehci_qh_t* qh)
{
	qh->used = 0;
}

void ehci_port_set(volatile uint32_t* reg,uint32_t data)
{
	uint32_t status = *reg;
	status |= data;
	status &= ~PORT_RWC;
	*reg = status;
}

void ehci_port_clr(volatile uint32_t* reg,uint32_t data)
{
	uint32_t status = *reg;
	status &= ~PORT_RWC;
	status &= ~data;
	status |= PORT_RWC & data;
	*reg = status;
}

uint32_t ehci_reset_port(ehci_controller_t* hc,uint32_t port)
{
	volatile uint32_t* reg = &hc->opRegs->ports[port];

	ehci_port_set(reg,PORT_RESET);
	timer_wait(50);
	ehci_port_clr(reg,PORT_RESET);

	uint32_t status = 0;
	for(uint32_t i=0;i<10;i++)
	{
		timer_wait(10);
		status = *reg;

		if(~status & PORT_CONNECT) break;

		if(status & (PORT_ENABLE_CHANGE | PORT_CSC))
		{
			ehci_port_clr(reg, PORT_ENABLE_CHANGE|PORT_CSC);
			continue;
		}

		if(status & PORT_ENABLE)
		{
			break;
		}
	}
	return status;
}

void ehci_init_qtd(ehci_qtd_t* qtd, ehci_qtd_t* prev, uint32_t toggle,
	uint32_t packetType, uint32_t len, void* data)
{
	if(prev)
	{
		prev->nextPtr = (uint32_t)qtd;
		prev->next = qtd;
	}

	qtd->nextPtr = Q_TYPE_END;
	qtd->alternatePtr = Q_TYPE_END;
	qtd->next = 0;

	qtd->token = 
		(toggle << QTD_TOG_SHIFT) |
		(len << QTD_LEN_SHIFT) |
		(3 << QTD_CERR_SHIFT) |
		(packetType << QTD_PID_SHIFT) |
		QTD_ACTIVE;

	uint32_t p = physical_address(data);
	dma_area(p,0x5000);
	qtd->buffer[0] = (uint32_t)p;
	qtd->extBuffer[0] = 0;//(uint32_t)(p>>32);
	p &= ~0xfff;

	for( int i = 1;i<4;i++)
	{
		p += 0x1000;
		qtd->buffer[i] = (uint32_t)p;
		qtd->extBuffer[i] = 0;//(uint32_t)(p>>32);
	}
}

void ehci_init_qh(ehci_qh_t* qh, usb_transfer_t* t, ehci_qtd_t* head,
	usb_device_t* parent, int interrupt, uint32_t speed, uint32_t addr,
	uint32_t endp, uint32_t maxSize)
{
	qh->transfer = t;

	uint32_t epch = 
		(maxSize << QH_CH_MPL_SHIFT) |
		QH_CH_DTC |
		(speed << QH_CH_EPS_SHIFT) |
		(endp << QH_CH_EPN_SHIFT) |
		addr;
	uint32_t epcap = 
		(1 << QH_CAP_MULT_SHIFT);

	if(!interrupt)
	{
		epch |= 5 << QH_CH_NAK_SHIFT;
	}

	if(speed != USB_HIGH_SPEED && parent)
	{
		if(interrupt)
		{
			epcap |= (0x1c << QH_CAP_COMPL_SHIFT);
		} else {
			epch |= QH_CH_CONTROL;
		}
		epcap |= 
			(parent->port << QH_CAP_HUB_PORT_SHIFT) |
			(parent->addr << QH_CAP_HUB_ADDR_SHIFT) ;
	}

	if(interrupt)
	{
		epcap |= (0x01 << QH_CAP_SCHED_SHIFT);
	}

	qh->epch = epch;
	qh->epcap = epcap;

	qh->nextPtr =(uint32_t)head;
	qh->token = 0;

	qh->qtdHead = head;
}

void ehci_insert_async_qh(ehci_qh_t* head, ehci_qh_t* qh)
{
// -1 tail    head +1 
//		   qh

	ehci_qh_t* tail = head->prev;

	qh->qhhlp = (uint32_t)head|Q_TYPE_QH;
	tail->qhhlp = (uint32_t)qh|Q_TYPE_QH;

	tail->next = qh;
	qh->prev = tail;
	qh->next = head;
	head->prev = qh;
}

void ehci_remove_qh(ehci_qh_t* qh)
{
	ehci_qh_t* prev = qh->prev;
	prev->qhhlp = qh->qhhlp;

	prev->next = qh->next;
	qh->next->prev = prev;
	qh->prev = qh->next = 0;
}

void ehci_process_qh(ehci_controller_t* hc, ehci_qh_t* qh)
{
	usb_transfer_t* t = qh->transfer;

	//kp("%x",qh->token);

	if(qh->token & QTD_HALT)
	{
		t->success = 0;
		t->complete = 1;
	} else if(qh->nextPtr & Q_TYPE_END)	{
		if(~qh->token & QTD_ACTIVE)
		{
			if(qh->token & QTD_DBE)
			{
				kp("Data Buffer Error");
			}
			if(qh->token & QTD_BABBLE)
			{
				kp("Babble Detected");
			}
			if(qh->token & QTD_XACT)
			{
				kp("Transaction Error");
			}
			if(qh->token & QTD_MMF)
			{
				kp("Missed Micro-Frame");
			}

			t->success = 1;
			t->complete = 1;
		}
	}

	if(t->complete)
	{
		qh->transfer = 0;

		if(t->success && t->endp)
		{
			t->endp->toggle ^=1;
		}

		ehci_remove_qh(qh);

		ehci_qtd_t* qtd = qh->qtdHead;
		while(qtd)
		{
			ehci_qtd_t *next = qtd->next;
			ehci_free_qtd(qtd);
			qtd = next;
		}
		ehci_free_qh(qh);
	}
}


void ehci_wait_for_qh(ehci_controller_t* hc, ehci_qh_t* qh)
{
	usb_transfer_t* t = qh->transfer;
	while(!t->complete)
	{
#if 1
		//clear_console();
		dbg_command(hc);
		dbg_status(hc);
		timer_wait(1000);
#endif
		ehci_process_qh(hc,qh);
	}
}

void ehci_dev_control(usb_device_t*dev, usb_transfer_t *t)
{
	ehci_controller_t* hc = (ehci_controller_t*)dev->hc;
	usb_request_t* req = t->req;

	uint32_t speed = dev->speed;
	uint32_t addr = dev->addr;
	uint32_t maxSize = dev->maxPacketSize;
	uint32_t type = req->type;
	uint32_t len = req->len;

	ehci_qtd_t* qtd = ehci_alloc_qtd(hc);

	ehci_qtd_t* head = qtd;
	ehci_qtd_t* prev = 0;

	uint32_t toggle = 0;
	uint32_t packetType = USB_PACKET_SETUP;
	uint32_t packetSize = sizeof(usb_request_t);

	ehci_init_qtd(qtd,prev,toggle,packetType,packetSize,req);
	prev = qtd;

	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

	uint8_t *it = (uint8_t*)t->data;
	uint8_t *end = it + len;
	while(it < end)
	{
		qtd = ehci_alloc_qtd(hc);
		
		toggle ^= 1;
		packetSize = end - it;
		if(packetSize > maxSize)
			packetSize = maxSize;

		ehci_init_qtd(qtd, prev,toggle, packetType,packetSize,it);

		it += packetSize;
		prev = qtd;
	}

	qtd = ehci_alloc_qtd(hc);

	toggle = 1;
	packetType = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;

	ehci_init_qtd(qtd, prev, toggle, packetType, 0, 0);

	ehci_qh_t* qh = ehci_alloc_qh(hc);
	ehci_init_qh(qh,t,head, dev->parent, 0, speed, addr, 0, maxSize);

	ehci_insert_async_qh(hc->asyncQH, qh);

	ehci_wait_for_qh(hc,qh);
}

void ehci_dev_intr(usb_device_t*dev, usb_transfer_t *t)
{

}

void ehci_probe(ehci_controller_t* hc)
{
	uint32_t numPort = hc->capRegs->hcsParams & (15<<0);
	kp("%d %d",numPort,hc->capRegs->hcsp.numPort);
	for(uint32_t port = 0;port != numPort;port++)
	{
		uint32_t status = ehci_reset_port(hc,port);

		if(status & PORT_ENABLE)
		{
			uint32_t speed = USB_HIGH_SPEED;

			kp("USB device detected at port: %d",port);

			usb_device_t * dev = usb_device_create();

			dev->parent = 0;
			dev->hc = hc;
			dev->port = port;
			dev->speed = speed;
			dev->maxPacketSize = 8;

			dev->hcControl = ehci_dev_control;
			dev->hcIntr = ehci_dev_intr;

			usb_device_init(dev);
		}
	}
}

void ehci_controller_poll_qh(ehci_controller_t* hc,ehci_qh_t* qh)
{

}

void ehci_controller_poll(usb_controller_t* usb)
{
	ehci_controller_t *hc = (ehci_controller_t*)usb->hc;

	ehci_controller_poll_qh(hc,(ehci_qh_t*)&hc->asyncQH);
	ehci_controller_poll_qh(hc,(ehci_qh_t*)&hc->periodicQH);
}

void ehci_irq(struct regs* r)
{
	kp("ehci irq");
}

#include "irq.h"

void ehci_init_2(uint32_t id, PCI_device_t* info)
{
	void* address = (void*)(uint32_t)(info->baseAddress[0]);
	uint8_t intr = info->interruptLine;

	kp("%d %d",intr, info->interruptPin);

	dma_address((uint32_t)address);

	ehci_controller_t *hc = (ehci_controller_t*)kmalloc(sizeof(ehci_controller_t));
	hc->capRegs = (ehci_cap_regs_t*)(uint32_t)(address);
	hc->opRegs = (ehci_op_regs_t*)(uint32_t)(address+hc->capRegs->capLength);

	uint32_t numPort = hc->capRegs->hcsp.numPort;

	kp("%d",hc->capRegs->hccp.bit64);

	irq_install_handler(intr,ehci_irq);

	hc->opRegs->command &= ~CMD_RUN;

	while(! (hc->opRegs->status & STS_HALT)) timer_wait(20);

	hc->opRegs->command = hc->opRegs->command | CMD_RESET;
	while((hc->opRegs->command & CMD_RESET)) timer_wait(20);
		//hc->opRegs->command = (8<< CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RUN;
	hc->opRegs->segment = 0;
	hc->opRegs->usbIntr = 1;
	hc->opRegs->frameList = 0;
	hc->opRegs->asyncListAddr = 0;
	hc->opRegs->command = hc->opRegs->command | CMD_RUN;

	while(hc->opRegs->status & STS_HALT);
	hc->opRegs->configFlag = 1;
	timer_wait(50);

	kp("lalal");
}

void ehci_init(uint32_t id, PCI_device_t* info)
{
	if(!(info->classCode == 0x0c &&
		info->subClassCode == 0x03 &&
		info->progIF == 0x20 ))
		return;

	kp("EHCI Initializing %d",id);

	ehci_init_2(id,info); return;

	void* address = (void*)(uint32_t)(info->baseAddress[0]&~0xf);

	uint8_t intr = info->interruptLine;

	dma_address((uint32_t)address);



	ehci_controller_t *hc = (ehci_controller_t*)kmalloc(sizeof(ehci_controller_t));
	hc->capRegs = (ehci_cap_regs_t*)(uint32_t)(address);
	hc->opRegs = (ehci_op_regs_t*)(uint32_t)(address+hc->capRegs->capLength);
	hc->frameList = (uint32_t*)kmalloc(1024*sizeof(uint32_t));
	hc->qhPool = (ehci_qh_t*)kzmalloc(8*sizeof(ehci_qh_t));
	hc->qtdPool = (ehci_qtd_t*)kzmalloc(32*sizeof(ehci_qtd_t));

	dma_area(physical_address(hc->qhPool),8*sizeof(ehci_qh_t));
	dma_area(physical_address(hc->qtdPool),32*sizeof(ehci_qtd_t));

	ehci_qh_t* qh = ehci_alloc_qh(hc);
	qh->qhhlp = (uint32_t)qh | Q_TYPE_QH ;
	qh->epch = QH_CH_HEAD; 
	qh->epcap = 0;
	qh->currentPtr = 0;
	qh->nextPtr = Q_TYPE_END;
	qh->alternatePtr = 0;
	qh->token = 0;
	for(int i=0;i!=5;i++)
	{
		qh->buffer[i] = 0;
		qh->extBuffer[i] = 0;
	}
	qh->transfer = 0;
	qh->prev = qh;
	qh->next = qh;

	hc->asyncQH = qh;

	qh = ehci_alloc_qh(hc);
	qh->qhhlp = Q_TYPE_END;
	qh->epch = 0;
	qh->epcap = 0;
	qh->currentPtr = 0;
	qh->nextPtr = Q_TYPE_END;
	qh->alternatePtr = 0;
	qh->token = 0;
	for(int i=0;i!=5;i++)
	{
		qh->buffer[i]=0;
		qh->extBuffer[i]=0;
	}
	qh->transfer = 0;
	qh->prev = qh;
	qh->next = qh;

	hc->periodicQH = qh;

	for(int i=0;i<1024;i++)
	{
		hc->frameList[i] = Q_TYPE_QH | (uint32_t)qh;
	}

	uint32_t eecp = (hc->capRegs->hccParams & HCCP_EECP_MASK ) >> HCCP_EECP_SHIFT;
	if( eecp >= 0x40 )
	{
		uint32_t legacy = pci_read_32(PCI_EXTRACT(id),eecp + USBLEGSUP);

		if( legacy & USBLEGSUP_HC_BIOS )
		{
			pci_write_32(PCI_EXTRACT(id),eecp + USBLEGSUP, legacy | USBLEGSUP_HC_OS);
			for(ever)
			{
				legacy = pci_read_32(PCI_EXTRACT(id),eecp + USBLEGSUP);
				if(~legacy & USBLEGSUP_HC_BIOS && legacy & USBLEGSUP_HC_OS)
				{
					break;
				}
			}
		}
	}

	hc->opRegs->usbIntr = 0;
	hc->opRegs->frameIndex = 0;
	hc->opRegs->frameList = physical_address((void*)hc->frameList);
	hc->opRegs->asyncListAddr = physical_address(hc->asyncQH);
	hc->opRegs->segment = 0;

	hc->opRegs->status = 0x3f;
	hc->opRegs->command = (8<< CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RUN;
	
	while(hc->opRegs->status & STS_HALT);

	hc->opRegs->configFlag = 1;

	timer_wait(50);

	ehci_probe(hc);

	usb_controller_t* controller = (usb_controller_t*)kmalloc(sizeof(usb_controller_t));
	controller->next = 0;
	controller->hc = hc;
	controller->poll = ehci_controller_poll;

	if(g_usb_controller)
	{
		usb_controller_t* next_usb = g_usb_controller;
		while(next_usb->next) next_usb = next_usb->next;
		next_usb->next = controller;
	} else {
		g_usb_controller = controller;
	}
	
		
}