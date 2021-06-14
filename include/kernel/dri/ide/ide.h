#pragma once

#include <dri/pci/pci.h>

enum ide_mode
{
    IDE_PCI_NATIVE,
    IDE_COMPAT,
};

struct ide_device
{
    enum ide_mode channel_mode[2];
    bool channel_mode_modifiable[2];
    bool supports_dma;
};

void ide_register();
void ide_print_device(struct ide_device *dev);
