#include <dri/ata_pio/ata_pio.h>
#include <dri/fs/ext2/ext2.h>
#include <kint.h>
#include <log.h>
#include <io.h>
#include <alloc.h>

inline uint ext2_block_size(struct ext2_superblock *sb)
{
	return 1024 << sb->block_size_shift;
}

void ext2_read_block(struct ext2_superblock *sb, void *buffer, uint block)
{
	uint block_size = ext2_block_size(sb) / 512;
	uint block_start = block_size * block;

	ata_pio_read_sectors(buffer, block_start, block_size);
}

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

struct ext2_block_group_descriptor ext2_load_block_group_descriptor(
	struct ext2_superblock *sb, uint block_group)
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

	const uint bgd_size = sizeof(struct ext2_block_group_descriptor);

	// Disk page that the BGD is on relative to the initial FILE SYSTEM block
	uint hd_page = block_group / (512 / bgd_size);
	// The offset from the beginning of that page the BGD is at
	uint bgd_offset = block_group % (512 / bgd_size);

	struct ext2_block_group_descriptor descriptors[512 / bgd_size];
	kassert(sizeof(descriptors) == 512, "Wrong BGD size");

	uint lba = (block_size / 512) * bgdt_block + hd_page;

	ata_pio_read_sectors(&descriptors, lba, 1);

	return descriptors[bgd_offset];
}

static void print_entry(uint inode, const char *name, void *sb)
{
	kprintf("%d\t %s\n", inode, name);

	struct ext2_inode in;

	if (ext2_find_inode(sb, inode, &in))
	{
		if ((in.mode & EXT2_F_TYPE) == EXT2_S_IFREG)
		{
			char buffer[65];
			uint read = ext2_read_inode(sb, &in, buffer, 64);
			buffer[read] = 0;

			kprintf("contents: %d\n'%s'\n", read, buffer);
		}
	}

	return;
}

void ext2_mount(struct fs_node *where)
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(DEBUG "EXT2 magic = 0x%x\n", sb.signature);
	
	// Read the root inode 2
	struct ext2_inode root;

	if (ext2_find_inode(&sb, 2, &root))
	{
		kprintf(OKAY "Found root inode 2 (size=0x%x, num_blocks=0x%x)\n", root.size, root.num_blocks);
		// kprintf(DEBUG "Root.mode = 0x%x\n", root.mode & 0xf000);
		kassert((root.mode & 0xf000) == EXT2_S_IFDIR, "Root (inode 2) is not a directory.");

		kprintf("ls /\n");
		kprintf("inode\t name\n");
		kprintf("--------------------\n");
		ext2_dir_ls(&sb, &root, print_entry, &sb);
	}
	else
	{
		kprintf(WARN "Failed to find root inode 2\n");
	}
}

bool ext2_valid_filesystem()
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(DEBUG "superblock signature is %d (0x%x)\n", sb.signature, sb.signature);

	return sb.signature == EXT2_SIGNATURE;
}

void ext2_write_superblock(struct ext2_superblock *sb)
{
	ushort *wp = (ushort *)sb;

	ata_pio_write_sectors(2, 2, wp);
}

void ext2_corrupt_superblock_for_fun()
{
	struct ext2_superblock sb = ext2_read_superblock();
	sb.signature = 0xDEAD;
	ext2_write_superblock(&sb);
}

bool ext2_find_inode(struct ext2_superblock *sb, uint number, struct ext2_inode *inode)
{
	if (number == 0)
		return false;

	uint block_group = (number - 1) / sb->inodes_per_block_group;
	uint local_index = (number - 1) % sb->inodes_per_block_group;

	// Load this from the block group descriptor table
	struct ext2_block_group_descriptor descriptor =
		ext2_load_block_group_descriptor(sb, block_group);

	// kprintf(DEBUG "Descriptor inode_table = 0x%x\n", descriptor.inode_table_start_block);

	// We need to figure out what FS block the inode is on, we know how many
	// inodes there are total in this BGD and the number per page, so this is
	// simple.

	const uint block_size = ext2_block_size(sb);

	const uint inodes_per_block = block_size / sizeof(struct ext2_inode);

	uint inode_block = local_index / inodes_per_block;
	uint inode_index = local_index % inodes_per_block;

	struct ext2_inode inodes[block_size / sizeof(struct ext2_inode)];

	ext2_read_block(sb, inodes, descriptor.inode_table_start_block +
								inode_block);

	*inode = inodes[inode_index];

	return true;
}

bool ext2_dir_ls(struct ext2_superblock *sb,
				 struct ext2_inode *dir,
				 void (*cb)(uint inode,
							const char *name,
							void *data),
				 void *data)
{
	if ((dir->mode & 0xf000) != EXT2_S_IFDIR)
		return false;

	for (int i = 0; i < dir->num_blocks; i++)
	{
		uchar buffer[ext2_block_size(sb)];
		ext2_read_inode_block(sb, dir, buffer, i);

		struct ext2_dirent *ent = (void *)buffer;

		// While there are files in this block
		while ((uint)ent < (uint)(buffer + ext2_block_size(sb)))
		{
			if (ent->inode == 0)
				return true;

			if (cb)
			{
				char name[257];

				memcpy(name, ent->name, ent->name_len);
				name[ent->name_len] = '\0';

				cb(ent->inode, name, data);
			}

			ent = (void *)(((uint)(void *)ent) + ent->rec_len);
		}
		// We ran out of files in this block, continue to the next one. This
		// works because files cannot span blocks
	}

	return true;
}

ssize_t ext2_read_inode(struct ext2_superblock *sb, struct ext2_inode *inode, void *buffer, ssize_t size)
{
	const uint block_size = ext2_block_size(sb);
	char transfer[block_size];

	uint fsize = MIN(inode->size, size);
	uint i;

	// Transfer full blocks straight to the output buffer
	for (i = 0; i < fsize / block_size; i++)
	{
		ext2_read_inode_block(sb, inode, buffer + i * block_size, i);
	}

	// If we have part of a block left over read it here first, then transfer what we need
	if (i * block_size < fsize)
	{
		uint remainder = fsize % block_size;

		ext2_read_inode_block(sb, inode, transfer, i);
		memcpy(buffer + i * block_size, transfer, remainder);
	}

	return fsize;
}

bool ext2_read_inode_block(struct ext2_superblock *sb,
						   struct ext2_inode *inode,
						   void *buffer,
						   uint block)
{
	if (block >= 12)
	{
		kprintf(ERROR "Sorry, EXT2 can only access the first 12 (direct) blocks "
					  "of an inode for now. Indirect look-up will be added later\n");
		kpanic("Invalid inode block");
	}

	uint block_address = inode->blocks[block];

	ext2_read_block(sb, buffer, block_address);

	return true;
}
