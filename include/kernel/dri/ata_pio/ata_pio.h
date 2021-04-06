#pragma once

#include <kint.h>

/**
 * ATA PIO Driver for Bluejay. This should be replaced by a proper
 * ATAPI driver later on, but for now this will do. It is going to
 * be horribly slow by virtue of having to pass all data through
 * in() and out().
 */

// status codes
enum
{
	ATA_BSY = 0x80, // Busy
	ATA_RDY = 0x40, // Ready for command
	ATA_DRQ = 0x08, // Ready for data
	ATA_DF = 0x20,
	ATA_ERR = 0x01, // Error code placed in error register
};

// IO ports
enum
{
	ATA_PORT_CMD = 0x1f7,
	ATA_PORT_DATA = 0x1f0,
	ATA_PORT_DRIVE_SEL = 0x1f6,
	ATA_PORT_SECTOR_COUNT = 0x1f2,
	ATA_PORT_LBA_LOW,
	ATA_PORT_LBA_MID,
	ATA_PORT_LBA_HIGH,
};

// Commands
enum
{
	ATA_CMD_READ = 0x20,
	ATA_CMD_WRITE = 0x30,
};

void ata_pio_wait_bsy();
void ata_pio_wait_drq();
void ata_pio_read_sectors(void *buffer, uint lba, uchar num_sectors);
void ata_pio_write_sectors(uint lba, uchar num_sectors, void *buffer);
uint ata_pio_get_error();

void test_ata_pio();
