#include <dri/pci/pci.h>
#include <dri/pci/vendors.h>
#include <io.h>
#include <log.h>

uint pci_config_readd(uchar bus, uchar slot, uchar func, uchar offset)
{
	uint address = (bus << 16) | (slot << 11) | (func << 8) | (offset << 2) | 0x80000000;

	outl(PCI_CONFIG_ADDRESS, address);

	return inl(PCI_CONFIG_DATA);
}

struct pci_vendor *pci_check_vendor(uchar bus, uchar slot, uchar func, ushort *v, ushort *d)
{
	uint vendor_device = pci_config_readd(bus, slot, func, 0);
	ushort vendor = vendor_device & 0xffff;

	if (v)
		*v = vendor;

	if (vendor != 0xffff)
	{
		if (d)
			*d = vendor_device >> 16;

		return pci_vendor_by_id(vendor);
	}
	return NULL;
}

struct pci_vendor *pci_vendor_by_id(ushort id)
{
	// Find vendor using binary search

	uint start = 0, 
		max = pci_num_vendors;

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
	kprintf("Enumerating PCI devices:\n");
	for (int bus = 0; bus < 0xff; bus++)
	{
		for (int slot = 0; slot < 32; slot++)
		{
			for (int func = 0; func < 8; func++)
			{
				ushort vendor, device;

				struct pci_vendor *v = pci_check_vendor(bus, slot, func, &vendor, &device);

				if (v)
				{
					kprintf("%d %d %d --- v:0x%x d:0x%x --- %s\n", bus, slot, func, vendor, device, v->name);
				}
			}
		}
	}
}
