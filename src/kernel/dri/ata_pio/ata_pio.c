#include <dri/ata_pio/ata_pio.h>

#include <io.h>
#include <log.h>
#include <pic.h>

/* TODO: Rewrite all of this to work with dri_ide in the case of multiple
 * devices */

static ushort test_buffer[256];

void ata_pio_wait_bsy()
{
	while (inb(ATA_PORT_CMD) & ATA_BSY)
	{
	}
}

void ata_pio_wait_drq()
{
	while (!(inb(ATA_PORT_CMD) & ATA_RDY))
	{
	}
}

uint ata_pio_get_error()
{
	return inb(ATA_ERR);
}

static void ata_pio_send_init(uint lba, uchar num_sectors)
{
	outb(ATA_PORT_DRIVE_SEL, 0xe0 | ((lba >> 24) & 0x0f));
	outb(0x1f1, 0x0); // waste some time
	outb(ATA_PORT_SECTOR_COUNT, num_sectors);
	outb(ATA_PORT_LBA_LOW, lba & 0xff);
	outb(ATA_PORT_LBA_MID, (lba >> 8) & 0xff);
	outb(ATA_PORT_LBA_HIGH, (lba >> 16) & 0xff);
}

void ata_pio_read_sectors(void *buffer, uint lba, uchar num_sectors)
{
	ushort *word_buffer = buffer;

	for (int sector = 0; sector < num_sectors; sector++)
	{
		ata_pio_wait_bsy();
		ata_pio_send_init(lba, num_sectors);
		outb(ATA_PORT_CMD, ATA_CMD_READ);

		ata_pio_wait_bsy();
		
		if (inb(ATA_PORT_STATUS) & ATA_ERR)
		{
			kprintf(ERROR "ATA_ERR on read\n");
			kpanic("Failed to read");
		}

		for (int i = 0; i < 256; i++)
		{
			word_buffer[i + sector * 256] = inw(ATA_PORT_DATA);
		}

		ata_pio_wait_bsy();
		outw(ATA_PORT_CMD, ATA_CMD_FLUSH_CACHE);
	}
}

void ata_pio_write_sectors(uint lba, uchar num_sectors, ushort *buffer)
{
	for (int sector = 0; sector < num_sectors; sector++)
	{
		ata_pio_wait_bsy();
		ata_pio_send_init(lba, num_sectors);
		outb(ATA_PORT_CMD, ATA_CMD_WRITE);

		ata_pio_wait_bsy();

		if (inb(ATA_PORT_STATUS) & ATA_ERR)
		{
			kprintf(ERROR "ATA_ERR on write\n");
			kpanic("Failed to write");
		}

		for (int i = 0; i < 256; i++)
		{
			outw(ATA_PORT_DATA, buffer[i + sector * 256]);
		}

		ata_pio_wait_bsy();
		outw(ATA_PORT_CMD, ATA_CMD_FLUSH_CACHE);
	}
}

static void print_buffer()
{
	for (int i = 0; i < 32; i++)
	{
		kprintf("%d ", test_buffer[i]);
		if ((i + 1) % 16 == 0)
			kprintf("\n");
	}
}

void test_ata_pio()
{
//	for (int i = 0; i < 2; i++)
	{
		kprintf(DEBUG "Writing 0s and reading back\n");

		// for (int i = 0; i < 256; i++)
		// 	test_buffer[i] = 0;
		// ata_pio_write_sectors(0, 1, test_buffer);

		ata_pio_read_sectors(test_buffer, 2, 1);
		print_buffer();
		kprintf(DEBUG "again...\n");
		ata_pio_read_sectors(test_buffer, 2, 1);
		print_buffer();

		for (int i = 0; i < 256; i++)
			test_buffer[i] = i;

		kprintf(DEBUG "Writing 0..256 and reading back\n");

		ata_pio_write_sectors(0, 1, test_buffer);

		ata_pio_read_sectors(test_buffer, 2, 1);
		print_buffer();
		kprintf(DEBUG "again...\n");
		ata_pio_read_sectors(test_buffer, 2, 1);
		print_buffer();
	}
}

void ata_pio_handle_irq(struct registers *regs)
{
	// TODO: check that this IRQ came from the hard drive
	
	// TODO: use a lock and inform the scheduler that the thread waiting for
	// this can stop sleeping

	// Acknowledge the IRQ
	uchar status = inw(ATA_PORT_STATUS);

	// kprintf(DEBUG "ATA PIO IRQ received %d (0x%x)\n", status, status);
}

void init_ata_pio()
{
	add_interrupt_handler(46, ata_pio_handle_irq);

/*
	// TODO: Consider adding this back.

	// 0xA0 for master, 0xB0 for slave
	outb(ATA_PORT_DRIVE_SEL, 0xA0);

	outb(ATA_PORT_LBA_LOW, 0);
	outb(ATA_PORT_LBA_MID, 0);
	outb(ATA_PORT_LBA_HIGH, 0);

	outb(ATA_PORT_CMD, ATA_CMD_IDENTIFY);

	if (inb(ATA_PORT_STATUS))
	{
		kprintf(OKAY "ATA drive exists\n");

		// Wait until either DRQ or ERR is set
		uchar status;
		while ((status = inb(ATA_PORT_STATUS)) & (ATA_DRQ | ATA_ERR))
		{
			if (status & ATA_ERR)
			{

			}
		}
	}
	else
	{
		kprintf(ERROR "ATA drive does not exist\n");
	}
*/
}
