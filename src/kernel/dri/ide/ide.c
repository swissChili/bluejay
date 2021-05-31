#include <dri/ide/ide.h>
#include <task.h>
#include <alloc.h>

struct ide_thread_data
{
    struct pci_device dev;
    uchar bus, slot, func;
};

bool ide_supports(struct pci_device *dev)
{
    return dev->class == 1 && dev->subclass == 1;
}

void ide_thread(struct ide_thread_data *data)
{
    kprintf("IDE driver thread starting: device=0x%x\n", data->dev.device_id);
}

void ide_init(struct pci_device dev, uchar bus, uchar slot, uchar func)
{
    struct ide_thread_data *data = malloc(sizeof(struct ide_thread_data));
    data->dev = dev;
    data->bus = bus;
    data->slot = slot;
    data->func = func;

    spawn_thread(ide_thread, data);
}

void ide_register()
{
    struct pci_device_driver dri =
    {
        .supports = ide_supports,
        .init = ide_init,
        .generic_name = "IDE Controller",
    };

    pci_register_device_driver(dri);
}
