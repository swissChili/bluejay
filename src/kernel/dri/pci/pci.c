#include <dri/pci/pci.h>
#include <dri/pci/vendors.h>
#include <io.h>
#include <log.h>

ushort pci_config_readw(uchar bus, uchar slot, uchar func, uchar offset)
{
	uint address = (bus << 16) | (slot << 11) | (func << 8) | (offset << 2) | 0x80000000;

	outl(PCI_CONFIG_ADDRESS, address);

	return inl(PCI_CONFIG_DATA);
}

struct pci_vendor *pci_check_vendor(uchar bus, uchar slot, uchar func, ushort *v)
{
	ushort vendor;

	if ((vendor = pci_config_readw(bus, slot, func, 0) != 0xffff))
	{
		// TODO: check device and return here as well
	}

	if (v)
		*v = vendor;

	return pci_vendor_by_id(vendor);
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
