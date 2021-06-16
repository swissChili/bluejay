#include <dri/fs/ext2/ext2.h>
#include <dri/ata_pio/ata_pio.h>
#include <kint.h>
#include <log.h>

struct ext2_superblock ext2_read_superblock()
{
	uchar buffer[512 * 2];
	ata_pio_read_sectors(buffer, 2, 2);

	struct ext2_superblock *sb = (void *)(buffer);
	return *sb;
}

void ext2_mount(struct fs_node *where)
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(INFO "EXT2 magic = 0x%x\n", sb.signature);
}

bool ext2_valid_filesystem()
{
	struct ext2_superblock sb = ext2_read_superblock();

	return sb.signature == EXT2_SIGNATURE;
}