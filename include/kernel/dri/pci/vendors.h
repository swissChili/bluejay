#pragma once

#include <kint.h>

struct pci_vendor
{
    ushort id;
    char *name;
};

extern struct pci_vendor pci_vendors[];
extern int pci_num_vendors;

struct pci_vendor *pci_vendor_by_id(ushort id);