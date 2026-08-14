#include "pci.h"
#include <efi.h>
#include <efilib.h>

static EFI_PCI_IO_PROTOCOL* get_pci_io(uint8_t bus, uint8_t slot, uint8_t func) {
    EFI_HANDLE* handles = NULL;
    UINTN handle_count = 0;

    EFI_GUID PciIoGuid = EFI_PCI_IO_PROTOCOL_GUID;

    if (uefi_call_wrapper(BS->LocateHandleBuffer, 5,
                          ByProtocol, &PciIoGuid,
                          NULL, &handle_count, &handles) != EFI_SUCCESS)
        return NULL;

    for (UINTN i = 0; i < handle_count; i++) {
        EFI_PCI_IO_PROTOCOL* io = NULL;
        if (uefi_call_wrapper(BS->HandleProtocol, 3,
                              handles[i], &PciIoGuid, (void**)&io) != EFI_SUCCESS)
            continue;

        UINTN b, s, f, function;
        io->GetLocation(io, &b, &s, &f, &function);

        if ((uint8_t)b == bus && (uint8_t)s == slot && (uint8_t)f == func)
            return io;
    }

    return NULL;
}

uint32_t pci_read_config32(uint8_t bus, uint8_t slot, uint8_t func, uint16_t offset) {
    EFI_PCI_IO_PROTOCOL* io = get_pci_io(bus, slot, func);
    if (!io)
        return 0xFFFFFFFF;

    uint32_t value = 0;
    io->Pci.Read(io, EfiPciIoWidthUint32, offset, 1, &value);
    return value;
}

void pci_scan_for_xhci(pci_device_t* out_dev, int* found) {
    *found = 0;

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint16_t slot = 0; slot < 32; slot++) {
            for (uint16_t func = 0; func < 8; func++) {

                uint32_t id = pci_read_config32(bus, slot, func, 0x00);
                if (id == 0xFFFFFFFF)
                    continue;

                uint16_t vendor = id & 0xFFFF;
                uint16_t device = (id >> 16) & 0xFFFF;

                uint32_t class_reg = pci_read_config32(bus, slot, func, 0x08);
                uint8_t class_code = (class_reg >> 24) & 0xFF;
                uint8_t subclass   = (class_reg >> 16) & 0xFF;
                uint8_t prog_if    = (class_reg >> 8) & 0xFF;

                if (class_code == 0x0C && subclass == 0x03 && prog_if == 0x30) {
                    out_dev->bus = bus;
                    out_dev->slot = slot;
                    out_dev->func = func;
                    out_dev->vendor_id = vendor;
                    out_dev->device_id = device;
                    out_dev->class_code = class_code;
                    out_dev->subclass = subclass;
                    out_dev->prog_if = prog_if;
                    *found = 1;
                    return;
                }
            }
        }
    }
}

