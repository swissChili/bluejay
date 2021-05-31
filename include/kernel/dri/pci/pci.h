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

struct pci_device_driver
{
	bool (* supports)(struct pci_device *dev);
	void (* init)(struct pci_device dev, uchar bus, uchar slot, uchar func);
	char *generic_name;
	uint loaded; // reserved
	struct pci_device dev; // reserved
};

// Call this first
void pci_init();
// Call this after registering drivers to load them
void pci_load();

// offset is in dwords
uint pci_config_readd(uchar bus, uchar slot, uchar func, uchar offset);
struct pci_device pci_check_device(uchar bus, uchar slot, uchar func);
void pci_print_devices();
void pci_register_device_driver(struct pci_device_driver driver);
void pci_print_drivers();
