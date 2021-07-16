#include <dri/ata_pio/ata_pio.h>
#include <dri/fs/ext2/ext2.h>
#include <kint.h>
#include <log.h>

struct ext2_superblock ext2_read_superblock()
{
	uchar buffer[512 * 2];
	ata_pio_read_sectors(buffer, 2, 2);

	struct ext2_superblock *sb = (void *)(buffer);
	return *sb;
}

uint ext2_num_block_groups(struct ext2_superblock *sb)
{
	// This is a mildly janky way of rounding up
	uint a = (sb->total_blocks - 1) / (sb->blocks_per_block_group + 1);
	uint b = (sb->total_inodes - 1) / (sb->inodes_per_block_group + 1);

	if (a == b)
	{
		return a;
	}
	else
	{
		kprintf(ERROR "EXT2 cannot find number of block groups, %d and %d "
					  "should equal.\n",
				a, b);
		kpanic("Corrupted filesystem");
	}
}

uint ext2_block_size(struct ext2_superblock *sb)
{
	return 1024 << sb->block_size_shift;
}

void ext2_load_block_group_descriptor_table(struct ext2_superblock *sb,
											uint num_block_groups)
{
	/**
	 * The BGDT (not to be confused with the GDT) is located the block after the
	 * superblock. On any block size EXCEPT 1024 (the minimum, remember that the
	 * block size is specified by X where 1024 << X is the real size) this is
	 * the second block (0-indexed, so 1). On 1024 this is the third block.
	 */
	uint bgdt_block = 1;
	uint block_size = ext2_block_size(sb);

	if (block_size == 1024)
		bgdt_block = 2;

	kprintf(DEBUG "BGDT block = %d block size = %d\n", bgdt_block, block_size);
}

void ext2_mount(struct fs_node *where)
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(DEBUG "EXT2 magic = 0x%x\n", sb.signature);

	uint num_block_groups = ext2_num_block_groups(&sb);
	ext2_load_block_group_descriptor_table(&sb, num_block_groups);
}

bool ext2_valid_filesystem()
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(DEBUG "superblock signature is %d (0x%x)\n", sb.signature, sb.signature);

	return sb.signature == EXT2_SIGNATURE;
}
