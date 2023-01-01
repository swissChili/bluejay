#include <dri/pci/pci.h>
#include <dri/pci/vendors.h>
#include <io.h>
#include <log.h>
#include <alloc.h>

static uint num_drivers, size_drivers;
static struct pci_device_driver *drivers;

uint pci_config_readd(uchar bus, uchar slot, uchar func, uchar offset)
{
	uint address =
		(bus << 16) | (slot << 11) | (func << 8) | (offset << 2) | 0x80000000;

	outl(PCI_CONFIG_ADDRESS, address);

	return inl(PCI_CONFIG_DATA);
}

struct pci_device pci_check_device(uchar bus, uchar slot, uchar func)
{

	uint vendor_device = pci_config_readd(bus, slot, func, 0);
	ushort vendor = vendor_device & 0xffff;

	struct pci_device device;
	device.valid = false;

	if (vendor != 0xffff)
	{
		device.valid = true;

		device.device_id = vendor_device >> 16;
		device.vendor = pci_vendor_by_id(vendor);

		// 3rd dword
		uint class_subclass = pci_config_readd(bus, slot, func, 2);
		device.class = class_subclass >> 24;
		device.subclass = (class_subclass >> 16) & 0xff;
		device.prog_if = (class_subclass >> 8) & 0xff;
	}

	return device;
}

struct pci_vendor *pci_vendor_by_id(ushort id)
{
	// Find vendor using binary search

	uint start = 0, max = pci_num_vendors;

	while (true)
	{
		if (max == start)
			// Can't find one
			return NULL;

		uint guess = (max - start) / 2 + start;

		if (pci_vendors[guess].id == id)
			return &pci_vendors[guess];
		else if (pci_vendors[guess].id > id)
			max = guess;
		else
			start = guess;
	}
}

void pci_print_devices()
{
	kprintf(INFO "Enumerating PCI devices:\n");
	for (int bus = 0; bus < 0xff; bus++)
	{
		for (int slot = 0; slot < 32; slot++)
		{
			for (int func = 0; func < 8; func++)
			{
				struct pci_device dev = pci_check_device(bus, slot, func);

				if (dev.valid)
				{
					kprintf(INFO "%d %d %d --- d:0x%x --- %d:%d:%d --- %s\n", bus,
							slot, func, dev.device_id, dev.class, dev.subclass,
							dev.prog_if, dev.vendor->name);
				}
			}
		}
	}
}

void pci_register_device_driver(struct pci_device_driver driver)
{
	if (num_drivers == size_drivers)
	{
		size_drivers += 8;
		drivers = realloc(drivers, sizeof(struct pci_device_driver) * size_drivers);
	}

	driver.loaded = 0;
	drivers[num_drivers++] = driver;
}

void pci_init()
{
	num_drivers = 0;
	size_drivers = 4;
	drivers = malloc(sizeof(struct pci_device_driver) * size_drivers);
}

void pci_load()
{
	for (int bus = 0; bus < 0xff; bus++)
	{
		for (int slot = 0; slot < 32; slot++)
		{
			for (int func = 0; func < 8; func++)
			{
				struct pci_device dev = pci_check_device(bus, slot, func);

				// Do any drivers support this?

				for (uint i = 0; i < num_drivers; i++)
				{
					if (drivers[i].supports(&dev))
					{
						drivers[i].loaded++;
						drivers[i].dev = dev;
						drivers[i].init(dev, bus, slot, func);
					}
				}
			}
		}
	}
}

void pci_print_drivers()
{
	kprintf(INFO "Enumerating PCI device drivers:\n");
	for (uint i = 0; i < num_drivers; i++)
	{
		for (uint j = 0; j < drivers[i].loaded; j++)
		{
			struct pci_device_driver d = drivers[i];
			kprintf(INFO "Driver: %s, vendor: %s\n", d.generic_name, d.dev.vendor->name);
		}
	}
}
