#ifndef PCI_H
#define PCI_H

#include <stdint.h>

typedef struct {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
} pci_device_t;

uint32_t pci_read_config32(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset);
void pci_scan_for_xhci(pci_device_t* out_dev, int* found);

#endif


