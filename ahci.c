#include "ahci.h"
#include <stdint.h>
#include <stddef.h>

/*
 * ============================================================
 * RAFA OS AHCI DRIVER
 * ============================================================
 *
 * x86-64
 * PCI AHCI
 * SATA HDD / SSD
 *
 * Current assumptions:
 *
 *   - PCI configuration mechanism #1
 *   - AHCI controller is PCI class 01 / subclass 06 / prog-if 01
 *   - MMIO addresses are identity mapped
 *   - DMA addresses can be obtained from virtual addresses directly
 *   - Polling is used instead of interrupts
 *   - One SATA port is supported
 *   - One PRDT entry is used
 *
 * ============================================================
 */

/* ============================================================
 * Basic I/O
 * ============================================================ */

static inline void outl(uint16_t port, uint32_t value)
{
    __asm__ volatile (
        "outl %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t value;

    __asm__ volatile (
        "inl %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

/* ============================================================
 * PCI
 * ============================================================ */

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static uint32_t pci_config_read32(
    uint8_t bus,
    uint8_t slot,
    uint8_t function,
    uint8_t offset
)
{
    uint32_t address;

    address =
        0x80000000U |
        ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);

    return inl(PCI_CONFIG_DATA);
}

static void pci_config_write32(
    uint8_t bus,
    uint8_t slot,
    uint8_t function,
    uint8_t offset,
    uint32_t value
)
{
    uint32_t address;

    address =
        0x80000000U |
        ((uint32_t)bus << 16) |
        ((uint32_t)slot << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

static uint16_t pci_config_read16(
    uint8_t bus,
    uint8_t slot,
    uint8_t function,
    uint8_t offset
)
{
    uint32_t value =
        pci_config_read32(
            bus,
            slot,
            function,
            offset
        );

    if (offset & 2)
        return (uint16_t)(value >> 16);

    return (uint16_t)value;
}

static uint8_t pci_config_read8(
    uint8_t bus,
    uint8_t slot,
    uint8_t function,
    uint8_t offset
)
{
    uint32_t value =
        pci_config_read32(
            bus,
            slot,
            function,
            offset
        );

    uint32_t shift =
        (uint32_t)(offset & 3) * 8;

    return (uint8_t)(value >> shift);
}

/* ============================================================
 * AHCI constants
 * ============================================================ */

#define AHCI_MAX_PORTS 32
#define AHCI_MAX_SLOTS 32

#define PCI_COMMAND_OFFSET 0x04

#define PCI_COMMAND_IO      (1U << 0)
#define PCI_COMMAND_MEMORY  (1U << 1)
#define PCI_COMMAND_MASTER  (1U << 2)

#define AHCI_GHC_AE         (1U << 31)

#define HBA_PxCMD_ST        (1U << 0)
#define HBA_PxCMD_CR        (1U << 15)
#define HBA_PxCMD_FRE       (1U << 4)
#define HBA_PxCMD_FR        (1U << 14)

#define HBA_PxTFD_BSY       (1U << 7)
#define HBA_PxTFD_DRQ       (1U << 3)
#define HBA_PxTFD_ERR       (1U << 0)

#define HBA_PxIS_TFES      (1U << 30)

#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1

#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SATAPI 2
#define AHCI_DEV_SEMB   3
#define AHCI_DEV_PM     4

/* FIS */

#define FIS_TYPE_REG_H2D 0x27

/* ATA commands */

#define ATA_CMD_IDENTIFY     0xEC
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_DMA_EXT 0x35

/* ============================================================
 * AHCI structures
 * ============================================================ */

/*
 * Received FIS area.
 *
 * AHCI requires 256-byte alignment.
 */
typedef struct
{
    uint8_t data[256];

} __attribute__((aligned(256))) HBA_FIS;

/*
 * Command header.
 *
 * 32 bytes.
 */
typedef struct
{
    uint8_t  cfl:5;
    uint8_t  a:1;
    uint8_t  w:1;
    uint8_t  p:1;

    uint8_t  r:1;
    uint8_t  b:1;
    uint8_t  c:1;
    uint8_t  rsv0:1;
    uint8_t  pmp:4;

    uint16_t prdtl;

    volatile uint32_t prdbc;

    uint32_t ctba;
    uint32_t ctbau;

    uint32_t rsv1[4];

} __attribute__((packed)) HBA_CMD_HEADER;

/*
 * Physical Region Descriptor.
 */
typedef struct
{
    uint32_t dba;
    uint32_t dbau;

    uint32_t rsv0;

    uint32_t dbc:22;
    uint32_t rsv1:9;
    uint32_t i:1;

} __attribute__((packed)) HBA_PRDT_ENTRY;

/*
 * Command table.
 *
 * CFIS  = 64 bytes
 * ACMD  = 16 bytes
 * RSV   = 48 bytes
 * PRDT  = 8 entries
 *
 * PRDT begins at offset 0x80.
 */
typedef struct
{
    uint8_t cfis[64];
    uint8_t acmd[16];
    uint8_t rsv[48];

    HBA_PRDT_ENTRY prdt_entry[8];

} __attribute__((packed, aligned(128))) HBA_CMD_TABLE;

/*
 * HBA port registers.
 */
typedef volatile struct
{
    uint32_t clb;
    uint32_t clbu;

    uint32_t fb;
    uint32_t fbu;

    uint32_t is;
    uint32_t ie;

    uint32_t cmd;
    uint32_t rsv0;

    uint32_t tfd;
    uint32_t sig;

    uint32_t ssts;
    uint32_t sctl;

    uint32_t serr;
    uint32_t sact;

    uint32_t ci;
    uint32_t sntf;

    uint32_t fbs;
    uint32_t devslp;

    uint32_t rsv1[10];

    uint32_t vendor[4];

} HBA_PORT;

/*
 * HBA memory registers.
 */
typedef volatile struct
{
    uint32_t cap;
    uint32_t ghc;
    uint32_t is;
    uint32_t pi;

    uint32_t vs;

    uint32_t ccc_ctl;
    uint32_t ccc_pts;

    uint32_t em_loc;
    uint32_t em_ctl;

    uint32_t cap2;
    uint32_t bohc;

    uint8_t rsv[0xA0 - 0x2C];

    uint8_t vendor[0x100 - 0xA0];

    HBA_PORT ports[AHCI_MAX_PORTS];

} HBA_MEM;

/* ============================================================
 * Driver memory
 * ============================================================ */

static HBA_MEM *ahci_hba = NULL;
static HBA_PORT *ahci_port = NULL;

static uint8_t ahci_port_number = 0;

static int ahci_ready = 0;

/*
 * Command list.
 *
 * AHCI command list must be 1 KiB aligned.
 */
static HBA_CMD_HEADER
ahci_cmd_headers[AHCI_MAX_SLOTS]
__attribute__((aligned(1024)));

/*
 * Received FIS.
 */
static HBA_FIS
ahci_fis
__attribute__((aligned(256)));

/*
 * Command table.
 */
static HBA_CMD_TABLE
ahci_cmd_table
__attribute__((aligned(128)));

/*
 * IDENTIFY buffer.
 */
static uint8_t
ahci_identify_buffer[512]
__attribute__((aligned(512)));

/* ============================================================
 * Physical address helper
 * ============================================================ */

static uint64_t ptr_phys(const void *ptr)
{
    /*
     * RAFA OS currently assumes identity mapping.
     */
    return (uint64_t)(uintptr_t)ptr;
}

/* ============================================================
 * Memory helpers
 * ============================================================ */

static void zero_memory(volatile void *ptr, size_t size)
{
    volatile uint8_t *p =
        (volatile uint8_t *)ptr;

    for (size_t i = 0; i < size; i++)
        p[i] = 0;
}

/* ============================================================
 * Port detection
 * ============================================================ */

static int ahci_check_port_type(HBA_PORT *port)
{
    uint32_t ssts = port->ssts;

    uint8_t det =
        (uint8_t)(ssts & 0x0F);

    uint8_t ipm =
        (uint8_t)((ssts >> 8) & 0x0F);

    if (det != HBA_PORT_DET_PRESENT)
        return AHCI_DEV_NULL;

    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch (port->sig)
    {
        case 0x00000101:
            return AHCI_DEV_SATA;

        case 0xEB140101:
            return AHCI_DEV_SATAPI;

        case 0xC33C0101:
            return AHCI_DEV_SEMB;

        case 0x96690101:
            return AHCI_DEV_PM;

        default:
            return AHCI_DEV_NULL;
    }
}

/* ============================================================
 * Stop command engine
 * ============================================================ */

static int ahci_stop_engine(HBA_PORT *port)
{
    /*
     * Clear ST.
     */
    port->cmd &= ~HBA_PxCMD_ST;

    /*
     * Clear FRE.
     */
    port->cmd &= ~HBA_PxCMD_FRE;

    /*
     * Wait for CR and FR to clear.
     */
    for (volatile uint32_t i = 0;
         i < 1000000;
         i++)
    {
        uint32_t cmd = port->cmd;

        if ((cmd & (HBA_PxCMD_CR | HBA_PxCMD_FR)) == 0)
            return AHCI_OK;
    }

    return AHCI_ERR_TIMEOUT;
}

/* ============================================================
 * Start command engine
 * ============================================================ */

static int ahci_start_engine(HBA_PORT *port)
{
    /*
     * Controller must not have CR/FR set.
     */
    for (volatile uint32_t i = 0;
         i < 1000000;
         i++)
    {
        uint32_t cmd = port->cmd;

        if ((cmd & (HBA_PxCMD_CR | HBA_PxCMD_FR)) == 0)
            break;
    }

    if (port->cmd & (HBA_PxCMD_CR | HBA_PxCMD_FR))
        return AHCI_ERR_TIMEOUT;

    /*
     * Enable FIS receive.
     */
    port->cmd |= HBA_PxCMD_FRE;

    /*
     * Start command engine.
     */
    port->cmd |= HBA_PxCMD_ST;

    return AHCI_OK;
}

/* ============================================================
 * Find command slot
 * ============================================================ */

static int ahci_find_free_slot(HBA_PORT *port)
{
    uint32_t slots =
        port->sact | port->ci;

    for (int i = 0; i < AHCI_MAX_SLOTS; i++)
    {
        if ((slots & (1U << i)) == 0)
            return i;
    }

    return -1;
}

/* ============================================================
 * Wait for port not busy
 * ============================================================ */

static int ahci_wait_not_busy(HBA_PORT *port)
{
    for (volatile uint32_t i = 0;
         i < 1000000;
         i++)
    {
        uint32_t tfd = port->tfd;

        if ((tfd & (HBA_PxTFD_BSY | HBA_PxTFD_DRQ)) == 0)
            return AHCI_OK;
    }

    return AHCI_ERR_TIMEOUT;
}

/* ============================================================
 * Issue command
 * ============================================================ */

static int ahci_issue_command(
    HBA_PORT *port,
    uint8_t command,
    uint64_t lba,
    uint32_t count,
    void *buffer,
    int write
)
{
    if (!port)
        return AHCI_ERR_NO_DEVICE;

    if (!buffer)
        return AHCI_ERR_ARGUMENT;

    if (count == 0)
        return AHCI_ERR_ARGUMENT;

    /*
     * This initial driver supports at most
     * 16 sectors per command.
     */
    if (count > 16)
        return AHCI_ERR_ARGUMENT;

    /*
     * Make sure the port isn't busy.
     */
    int result =
        ahci_wait_not_busy(port);

    if (result != AHCI_OK)
        return result;

    /*
     * Clear pending interrupt/error bits.
     */
    port->is = 0xFFFFFFFF;

    /*
     * Find command slot.
     */
    int slot =
        ahci_find_free_slot(port);

    if (slot < 0)
        return AHCI_ERR_TIMEOUT;

    /*
     * Command header.
     */
    HBA_CMD_HEADER *header =
        &ahci_cmd_headers[slot];

    /*
     * Clear header.
     */
    zero_memory(
        (volatile void *)header,
        sizeof(HBA_CMD_HEADER)
    );

    /*
     * Command FIS length:
     *
     * 20 bytes / 4 = 5 DWORDs.
     */
    header->cfl = 5;

    /*
     * Write bit.
     */
    header->w =
        write ? 1 : 0;

    /*
     * Prefetchable.
     */
    header->p = 1;

    /*
     * One PRDT entry.
     */
    header->prdtl = 1;

    /*
     * Command table address.
     */
    uint64_t ctba =
        ptr_phys(&ahci_cmd_table);

    header->ctba =
        (uint32_t)ctba;

    header->ctbau =
        (uint32_t)(ctba >> 32);

    /*
     * Clear command table.
     */
    zero_memory(
        (volatile void *)&ahci_cmd_table,
        sizeof(HBA_CMD_TABLE)
    );

    HBA_CMD_TABLE *table =
        &ahci_cmd_table;

    /*
     * ========================================================
     * Register Host-to-Device FIS
     * ========================================================
     */

    uint8_t *fis =
        table->cfis;

    fis[0] =
        FIS_TYPE_REG_H2D;

    /*
     * C bit = 1
     *
     * This FIS contains an ATA command.
     */
    fis[1] =
        (1U << 7);

    /*
     * ATA command.
     */
    fis[2] =
        command;

    /*
     * Features = 0.
     */
    fis[3] = 0;

    /*
     * LBA bits 0-23.
     */
    fis[4] =
        (uint8_t)(lba);

    fis[5] =
        (uint8_t)(lba >> 8);

    fis[6] =
        (uint8_t)(lba >> 16);

    /*
     * Device register.
     *
     * LBA mode.
     */
    fis[7] =
        0x40;

    /*
     * LBA bits 24-47.
     */
    fis[8] =
        (uint8_t)(lba >> 24);

    fis[9] =
        (uint8_t)(lba >> 32);

    fis[10] =
        (uint8_t)(lba >> 40);

    /*
     * Features high.
     */
    fis[11] = 0;

    /*
     * Sector count.
     */
    fis[12] =
        (uint8_t)(count);

    fis[13] =
        (uint8_t)(count >> 8);

    /*
     * ========================================================
     * PRDT
     * ========================================================
     */

    uint64_t buffer_phys =
        ptr_phys(buffer);

    table->prdt_entry[0].dba =
        (uint32_t)buffer_phys;

    table->prdt_entry[0].dbau =
        (uint32_t)(buffer_phys >> 32);

    /*
     * Byte count minus one.
     */
    table->prdt_entry[0].dbc =
        (count * 512U) - 1;

    /*
     * Interrupt on completion.
     *
     * Interrupts are disabled globally at the port,
     * but this is still a valid PRDT setting.
     */
    table->prdt_entry[0].i = 1;

    /*
     * Ensure pending errors are cleared.
     */
    port->is = 0xFFFFFFFF;

    /*
     * Issue command.
     */
    port->ci =
        (1U << slot);

    /*
     * Poll.
     */
    while (port->ci & (1U << slot))
    {
        /*
         * Task file error.
         */
        if (port->is & HBA_PxIS_TFES)
            return AHCI_ERR_DEVICE;

        /*
         * ATA ERR bit.
         */
        if (port->tfd & HBA_PxTFD_ERR)
            return AHCI_ERR_DEVICE;
    }

    /*
     * Final error check.
     */
    if (port->is & HBA_PxIS_TFES)
        return AHCI_ERR_DEVICE;

    if (port->tfd & HBA_PxTFD_ERR)
        return AHCI_ERR_DEVICE;

    return AHCI_OK;
}

/* ============================================================
 * IDENTIFY DEVICE
 * ============================================================ */

int ahci_identify(uint16_t *identify_buffer)
{
    if (!ahci_ready)
        return AHCI_ERR_NO_DEVICE;

    if (!identify_buffer)
        return AHCI_ERR_ARGUMENT;

    zero_memory(
        ahci_identify_buffer,
        sizeof(ahci_identify_buffer)
    );

    int result =
        ahci_issue_command(
            ahci_port,
            ATA_CMD_IDENTIFY,
            0,
            1,
            ahci_identify_buffer,
            0
        );

    if (result != AHCI_OK)
        return result;

    uint16_t *src =
        (uint16_t *)ahci_identify_buffer;

    for (int i = 0; i < 256; i++)
        identify_buffer[i] = src[i];

    return AHCI_OK;
}

/* ============================================================
 * READ
 * ============================================================ */

int ahci_read(
    uint64_t lba,
    uint32_t count,
    void *buffer
)
{
    if (!ahci_ready)
        return AHCI_ERR_NO_DEVICE;

    if (!buffer)
        return AHCI_ERR_ARGUMENT;

    return ahci_issue_command(
        ahci_port,
        ATA_CMD_READ_DMA_EXT,
        lba,
        count,
        buffer,
        0
    );
}

/* ============================================================
 * WRITE
 * ============================================================ */

int ahci_write(
    uint64_t lba,
    uint32_t count,
    const void *buffer
)
{
    if (!ahci_ready)
        return AHCI_ERR_NO_DEVICE;

    if (!buffer)
        return AHCI_ERR_ARGUMENT;

    return ahci_issue_command(
        ahci_port,
        ATA_CMD_WRITE_DMA_EXT,
        lba,
        count,
        (void *)buffer,
        1
    );
}

/* ============================================================
 * Find AHCI controller
 * ============================================================ */

static int ahci_find_controller(
    uint8_t *out_bus,
    uint8_t *out_slot,
    uint8_t *out_function
)
{
    for (uint16_t bus = 0;
         bus < 256;
         bus++)
    {
        for (uint8_t slot = 0;
             slot < 32;
             slot++)
        {
            for (uint8_t function = 0;
                 function < 8;
                 function++)
            {
                uint32_t id =
                    pci_config_read32(
                        (uint8_t)bus,
                        slot,
                        function,
                        0x00
                    );

                /*
                 * No PCI device.
                 */
                if (id == 0xFFFFFFFF)
                    continue;

                uint16_t vendor =
                    (uint16_t)(id & 0xFFFF);

                uint16_t device =
                    (uint16_t)(id >> 16);

                if (vendor == 0xFFFF)
                    continue;

                uint8_t class_code =
                    pci_config_read8(
                        (uint8_t)bus,
                        slot,
                        function,
                        0x0B
                    );

                uint8_t subclass =
                    pci_config_read8(
                        (uint8_t)bus,
                        slot,
                        function,
                        0x0A
                    );

                uint8_t prog_if =
                    pci_config_read8(
                        (uint8_t)bus,
                        slot,
                        function,
                        0x09
                    );

                /*
                 * AHCI:
                 *
                 * Base class     01
                 * Subclass        06
                 * Programming IF  01
                 */
                if (class_code == 0x01 &&
                    subclass == 0x06 &&
                    prog_if == 0x01)
                {
                    (void)device;

                    *out_bus =
                        (uint8_t)bus;

                    *out_slot =
                        slot;

                    *out_function =
                        function;

                    return AHCI_OK;
                }
            }
        }
    }

    return AHCI_ERR_NO_DEVICE;
}

/* ============================================================
 * Enable PCI bus mastering
 * ============================================================ */

static void ahci_enable_pci(
    uint8_t bus,
    uint8_t slot,
    uint8_t function
)
{
    uint16_t command =
        pci_config_read16(
            bus,
            slot,
            function,
            PCI_COMMAND_OFFSET
        );

    /*
     * Enable:
     *
     * Memory Space
     * Bus Mastering
     */
    command |=
        PCI_COMMAND_MEMORY |
        PCI_COMMAND_MASTER;

    pci_config_write32(
        bus,
        slot,
        function,
        PCI_COMMAND_OFFSET,
        (pci_config_read32(
            bus,
            slot,
            function,
            PCI_COMMAND_OFFSET
        ) & 0xFFFF0000U) |
        command
    );
}

/* ============================================================
 * Read AHCI BAR5
 * ============================================================ */

static uint64_t ahci_read_abar(
    uint8_t bus,
    uint8_t slot,
    uint8_t function
)
{
    uint32_t bar5 =
        pci_config_read32(
            bus,
            slot,
            function,
            0x24
        );

    /*
     * BAR is not implemented.
     */
    if (bar5 == 0 ||
        bar5 == 0xFFFFFFFF)
    {
        return 0;
    }

    /*
     * Memory BAR type:
     *
     * bits 2:1
     *
     * 00 = 32-bit
     * 10 = 64-bit
     */
    uint32_t type =
        (bar5 >> 1) & 3;

    if (type == 2)
    {
        uint32_t bar5_high =
            pci_config_read32(
                bus,
                slot,
                function,
                0x28
            );

        return
            ((uint64_t)bar5_high << 32) |
            ((uint64_t)bar5 & ~0xFULL);
    }

    return
        (uint64_t)(bar5 & ~0xFULL);
}

/* ============================================================
 * Initialize
 * ============================================================ */

int ahci_init(void)
{
    uint8_t bus;
    uint8_t slot;
    uint8_t function;

    /*
     * Reset state.
     */
    ahci_ready = 0;
    ahci_hba = NULL;
    ahci_port = NULL;

    /*
     * Find PCI AHCI controller.
     */
    int result =
        ahci_find_controller(
            &bus,
            &slot,
            &function
        );

    if (result != AHCI_OK)
        return AHCI_ERR_NO_DEVICE;

    /*
     * Enable PCI memory access and bus mastering.
     */
    ahci_enable_pci(
        bus,
        slot,
        function
    );

    /*
     * Get AHCI ABAR.
     */
    uint64_t abar =
        ahci_read_abar(
            bus,
            slot,
            function
        );

    if (abar == 0)
        return AHCI_ERR_NO_DEVICE;

    /*
     * Current RAFA OS assumption:
     *
     * physical address == usable virtual address.
     */
    ahci_hba =
        (HBA_MEM *)(uintptr_t)abar;

    /*
     * Enable AHCI mode.
     */
    ahci_hba->ghc |= AHCI_GHC_AE;

    /*
     * Disable global interrupts.
     *
     * This driver uses polling.
     */
    ahci_hba->ghc &= ~(1U << 1);

    /*
     * Clear global interrupt status.
     */
    ahci_hba->is = 0xFFFFFFFF;

    /*
     * Port implemented bitmap.
     */
    uint32_t pi =
        ahci_hba->pi;

    /*
     * Find a SATA port.
     */
    for (uint8_t i = 0;
         i < AHCI_MAX_PORTS;
         i++)
    {
        if ((pi & (1U << i)) == 0)
            continue;

        HBA_PORT *port =
            (HBA_PORT *)&ahci_hba->ports[i];

        int type =
            ahci_check_port_type(port);

        if (type != AHCI_DEV_SATA)
            continue;

        /*
         * Found SATA port.
         */
        ahci_port =
            port;

        ahci_port_number =
            i;

        /*
         * Stop command engine.
         */
        result =
            ahci_stop_engine(port);

        if (result != AHCI_OK)
        {
            ahci_port = NULL;
            return result;
        }

        /*
         * Clear port errors.
         */
        port->serr = 0xFFFFFFFF;
        port->is = 0xFFFFFFFF;

        /*
         * Command list.
         */
        uint64_t clb =
            ptr_phys(ahci_cmd_headers);

        port->clb =
            (uint32_t)clb;

        port->clbu =
            (uint32_t)(clb >> 32);

        /*
         * Received FIS.
         */
        uint64_t fb =
            ptr_phys(&ahci_fis);

        port->fb =
            (uint32_t)fb;

        port->fbu =
            (uint32_t)(fb >> 32);

        /*
         * Clear command list.
         */
        zero_memory(
            ahci_cmd_headers,
            sizeof(ahci_cmd_headers)
        );

        /*
         * Clear FIS.
         */
        zero_memory(
            &ahci_fis,
            sizeof(ahci_fis)
        );

        /*
         * Disable port interrupts.
         */
        port->ie = 0;

        /*
         * Start command engine.
         */
        result =
            ahci_start_engine(port);

        if (result != AHCI_OK)
        {
            ahci_port = NULL;
            return result;
        }

        /*
         * Driver is ready.
         */
        ahci_ready = 1;

        return AHCI_OK;
    }

    ahci_port = NULL;

    return AHCI_ERR_PORT;
}

/* ============================================================
 * Availability
 * ============================================================ */

int ahci_available(void)
{
    return ahci_ready;
}

/* ============================================================
 * Optional port information
 * ============================================================ */

uint8_t ahci_get_port(void)
{
    return ahci_port_number;
}

