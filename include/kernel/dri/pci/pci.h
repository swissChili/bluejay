#pragma once

#include <kint.h>
#include <dri/pci/vendors.h>

enum
{
	PCI_CONFIG_ADDRESS = 0xCF8,
	PCI_CONFIG_DATA = 0xCFC,
};

struct pci_device
{
	struct pci_vendor *vendor;
	uchar class, subclass, prog_if;
	ushort device_id;
	bool valid;
};

// offset is in dwords
uint pci_config_readd(uchar bus, uchar slot, uchar func, uchar offset);
struct pci_device pci_check_device(uchar bus, uchar slot, uchar func);
void pci_print_devices();
