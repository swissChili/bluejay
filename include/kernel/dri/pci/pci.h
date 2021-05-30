#pragma once

#include <kint.h>
#include <dri/pci/vendors.h>

enum
{
	PCI_CONFIG_ADDRESS = 0xCF8,
	PCI_CONFIG_DATA = 0xCFC,
};

uint pci_config_readd(uchar bus, uchar slot, uchar func, uchar offset);
struct pci_vendor *pci_check_vendor(uchar bus, uchar slot, uchar func, uint *v);
void pci_print_devices();
