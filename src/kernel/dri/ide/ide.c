#include <alloc.h>
#include <dri/ata_pio/ata_pio.h>
#include <dri/ide/ide.h>
#include <log.h>
#include <task.h>

struct ide_thread_data
{
	struct pci_device dev;
	uchar bus, slot, func;
};

bool ide_supports(struct pci_device *dev)
{
	return dev->class == 1 && dev->subclass == 1;
}

void ide_print_device(struct ide_device *dev)
{
	kprintf(INFO "<ide-device dma=%b>\n", dev->supports_dma);
}

void ide_thread(struct ide_thread_data *data)
{
	struct ide_device dev;

	uchar p = data->dev.prog_if;
	dev.channel_mode[0] = p & 1;
	dev.channel_mode_modifiable[0] = p & (1 << 1);
	dev.channel_mode[1] = p & (1 << 2);
	dev.channel_mode_modifiable[1] = p & (1 << 3);
	dev.supports_dma = p & (1 << 7);

	ide_print_device(&dev);

	// TODO: pass ATA PIO driver information about the IDE device (i.e. what
	// ports to use)
	init_ata_pio();

	kprintf(OKAY "Set up ATA PIO\n");

	free(data);
}

void ide_init(struct pci_device dev, uchar bus, uchar slot, uchar func)
{
	struct ide_thread_data *data = malloc(sizeof(struct ide_thread_data));
	data->dev = dev;
	data->bus = bus;
	data->slot = slot;
	data->func = func;

	ide_thread(data);
}

void ide_register()
{
	struct pci_device_driver dri = {
		.supports = ide_supports,
		.init = ide_init,
		.generic_name = "IDE Controller",
	};

	pci_register_device_driver(dri);
}
