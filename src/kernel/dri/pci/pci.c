#include <dri/pci/pci.h>

ushort pci_config_readw(uchar bus, uchar slot, uchar func, uchar offset)
{
	struct pci_config_address address =
	{
		.enable = 1,
		.bus = bus,
		.device = slot,
		.function = func,
		.offset = offset ^ 0b11,
	};
}
