#include <dri/ata_pio/ata_pio.h>

#include <io.h>
#include <log.h>

static uchar test_buffer[256 * 4];

void ata_pio_wait_bsy()
{
	while (inb(ATA_PORT_CMD) & ATA_BSY)
	{}
}

void ata_pio_wait_drq()
{
	while (!(inb(ATA_PORT_CMD) & ATA_RDY))
	{}
}

static void ata_pio_send_init(uint lba, uchar num_sectors)
{
	outb(ATA_PORT_DRIVE_SEL, 0xe0 | (lba >> 24));
	outb(ATA_PORT_SECTOR_COUNT, num_sectors);
	outb(ATA_PORT_LBA_LOW, lba & 0xff);
	outb(ATA_PORT_LBA_MID, (lba >> 8) & 0xff);
	outb(ATA_PORT_LBA_HIGH, (lba >> 16) & 0xff);
}

void ata_pio_read_sectors(uchar *buffer, uint lba, uchar num_sectors)
{
	ata_pio_wait_bsy();

	ata_pio_send_init(lba, num_sectors);
	outb(ATA_PORT_CMD, ATA_CMD_READ);

	for (int i = 0; i < num_sectors; i++)
	{
		ata_pio_wait_bsy();
		ata_pio_wait_drq();

		for (int j = 0; j < 256; j++)
			buffer[i * 256 + j] = inb(ATA_PORT_DATA);
	}
}

void ata_pio_write_sectors(uint lba, uchar num_sectors, uchar *buffer)
{
	ata_pio_wait_bsy();

	ata_pio_send_init(lba, num_sectors);
	outb(ATA_PORT_CMD, ATA_CMD_WRITE);

	for (int i = 0; i < num_sectors; i++)
	{
		ata_pio_wait_bsy();
		ata_pio_wait_drq();

		for (int j = 0; j < 256; j++)
			outb(ATA_PORT_DATA, buffer[i * 256 + j]);
	}
}

void test_ata_pio()
{
	kprintf("Going to ata_pio_read_sectors\n");
	ata_pio_read_sectors(test_buffer, 0, 4);

	for (int i = 0; i < 256 * 4; i += 64)
		kprintf("Byte %d = %d\n", test_buffer[i]);
}
