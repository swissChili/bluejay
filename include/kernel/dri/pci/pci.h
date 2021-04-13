#pragma once

#include <kint.h>

enum
{
	PCI_CONFIG_ADDRESS = 0xCF8,
	PCI_CONFIG_DATA = 0xCFC,
};

struct pci_config_address
{
	union
	{
		struct
		{
			int enable : 1;
			int reserved : 7;
			int bus : 8;
			int device : 5;
			int function : 3;
			// Offset into the 256 byte register. Least significant two bits must be zero.
			int offset : 8;
		};

		uint packed;
	};
	
} __attribute__((packed));

ushort pci_config_readw(uchar bus, uchar slot, uchar func, uchar offset);
