#ifndef AHCI_H
#define AHCI_H

#include <stdint.h>

#define AHCI_OK             0
#define AHCI_ERR_NO_DEVICE -1
#define AHCI_ERR_TIMEOUT   -2
#define AHCI_ERR_DEVICE    -3
#define AHCI_ERR_PORT      -4
#define AHCI_ERR_ARGUMENT  -5

int ahci_init(void);

/*
 * Read `count` sectors beginning at LBA into buffer.
 *
 * buffer must be DMA accessible.
 * For the initial implementation, keep buffer below 4 GiB.
 */
int ahci_read(uint64_t lba, uint32_t count, void *buffer);

/*
 * Write sectors to the SATA device.
 */
int ahci_write(uint64_t lba, uint32_t count, const void *buffer);

/*
 * Read the IDENTIFY DEVICE information.
 */
int ahci_identify(uint16_t *identify_buffer);

/*
 * Returns 1 if AHCI/SATA was successfully initialized.
 */
int ahci_available(void);

#endif

