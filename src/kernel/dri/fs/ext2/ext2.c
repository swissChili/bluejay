#include <alloc.h>
#include <dri/ata_pio/ata_pio.h>
#include <dri/fs/ext2/ext2.h>
#include <dri/fs/ext2/ext2_vfs.h>
#include <vfs.h>
#include <io.h>
#include <kint.h>
#include <log.h>

#define F(f,t) [EXT2_S_##f >> 12] = EXT2_FT_##t,
const uchar ext2_s_to_ft[] =
{
	0,
	F(IFSOCK, SOCK)
	F(IFLINK, SYMLINK)
	F(IFREG, REGULAR_FILE)
	F(IFBLK, BLKDEV)
	F(IFDIR, DIR)
	F(IFCHR, CHRDEV)
	F(IFIFO, FIFO)
};
#undef F

void ext2_read_block(struct ext2_superblock *sb, void *buffer, uint block)
{
	uint block_size = ext2_block_size(sb) / 512;
	uint block_start = block_size * block;

	ata_pio_read_sectors(buffer, block_start, block_size);
}

void ext2_write_block(struct ext2_superblock *sb, void *buffer, uint block)
{
	uint block_size = ext2_block_size(sb) / 512;
	uint block_start = block_size * block;

	ata_pio_write_sectors(block_start, block_size, buffer);
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
	uint a = IDIV_CEIL(sb->total_blocks, sb->blocks_per_block_group);
	uint b = IDIV_CEIL(sb->total_inodes, sb->inodes_per_block_group);

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

void ext2_write_bgd(struct ext2_superblock *sb, uint block_group,
					struct ext2_block_group_descriptor *d)
{
	ext2_load_or_write_bgd(sb, block_group, d, true);
}

struct ext2_block_group_descriptor ext2_load_bgd(
	struct ext2_superblock *sb, uint block_group)
{
	struct ext2_block_group_descriptor bgd;
	ext2_load_or_write_bgd(sb, block_group, &bgd, false);

	return bgd;
}

void ext2_load_or_write_bgd(struct ext2_superblock *sb,
							uint block_group,
							struct ext2_block_group_descriptor *d,
							bool set)
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

	ata_pio_read_sectors(descriptors, lba, 1);

	if (set)
	{
		descriptors[bgd_offset] = *d;
		ata_pio_write_sectors(lba, 1, (ushort *)descriptors);
	}
	else
	{
		*d = descriptors[bgd_offset];
	}
}

static bool print_entry(uint inode, const char *name, uint l, void *sb)
{
	kprintf("%d\t %s\n", inode, name);

	return true;

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

	return true;
}

void ext2_mount(struct fs_node *where)
{

	struct ext2_superblock *sb = malloc(sizeof(struct ext2_superblock));
	*sb = ext2_read_superblock();

	struct fs_node *ext2_root = ext2_inode2vfs(sb, 2, "/", 1);
	uint err;

	if ((err = fs_mount(where, ext2_root)))
	{
		kprintf(WARN "Failed to mount EXT2: error %d\n", err);
	}
}

bool ext2_valid_filesystem()
{
	struct ext2_superblock sb = ext2_read_superblock();

	kprintf(DEBUG "superblock signature is %d (0x%x)\n", sb.signature,
			sb.signature);

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

bool ext2_get_or_set_inode(struct ext2_superblock *sb, uint number,
						   struct ext2_inode *inode, bool set)
{
	if (number == 0)
		return false;

	uint block_group = (number - 1) / sb->inodes_per_block_group;
	uint local_index = (number - 1) % sb->inodes_per_block_group;

	// Load this from the block group descriptor table
	struct ext2_block_group_descriptor descriptor =
		ext2_load_bgd(sb, block_group);

	// We need to figure out what FS block the inode is on, we know how many
	// inodes there are total in this BGD and the number per page, so this is
	// simple.

	const uint block_size = ext2_block_size(sb);

	const uint inodes_per_block =
		block_size / sizeof(struct ext2_inode);

	uint inode_block = local_index / inodes_per_block;
	uint inode_index = local_index % inodes_per_block;

	struct ext2_inode inodes[block_size / sizeof(struct ext2_inode)];

	const uint fs_block = descriptor.inode_table_start_block +
		inode_block;

	ext2_read_block(sb, inodes, fs_block);

	if (set)
	{
		inodes[inode_index] = *inode;

		ext2_write_block(sb, inodes, fs_block);
	}
	else
	{
		*inode = inodes[inode_index];
	}

	return true;
}

bool ext2_find_inode(struct ext2_superblock *sb, uint number,
					 struct ext2_inode *inode)
{
	return ext2_get_or_set_inode(sb, number, inode, false);
}

bool ext2_set_inode(struct ext2_superblock *sb, uint number,
					struct ext2_inode *inode)
{
	return ext2_get_or_set_inode(sb, number, inode, true);
}

bool ext2_dir_ls(struct ext2_superblock *sb, struct ext2_inode *dir,
				 bool (*cb)(uint inode, const char *name, uint name_len,
							void *data),
				 void *data)
{
	if ((dir->mode & 0xf000) != EXT2_S_IFDIR)
		return false;

	for (uint i = 0; i < dir->num_blocks; i++)
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
				char name[256];
				uint name_len = MIN(ent->name_len, 255);

				memcpy(name, ent->name, name_len);
				name[name_len] = '\0';

				if (!cb(ent->inode, name, name_len, data))
					return true;
			}

			ent = (void *)(((uint)(void *)ent) + ent->rec_len);
		}
		// We ran out of files in this block, continue to the next one. This
		// works because files cannot span blocks
	}

	return true;
}

static void ext2_show_dirent(struct ext2_dirent *ent)
{
	char name[ent->name_len + 1];
	memcpy(name, ent->name, ent->name_len);
	name[ent->name_len] = '\0';

	kprintf(DEBUG "<ent ft=%p, i=%d, s=%s, l=%d>\n", ent->file_type, ent->inode,
			ent->name, ent->rec_len);
}

bool ext2_hard_link(struct ext2_superblock *sb, struct ext2_inode *dir,
                    char *name, uint name_len, uint inode)
{
	struct ext2_inode in;

	if (!ext2_dir_contains(sb, dir, name, name_len) &&
		ext2_find_inode(sb, inode, &in))
	{
		if ((dir->mode & 0xf000) != EXT2_S_IFDIR)
		{
			return false;
		}

		// Increment the reference count to this inode
		in.links_count++;
		ext2_set_inode(sb, inode, &in);

		// Insert it into the directory
		uchar type = EXT2_S_TO_FT(in.mode);
		ext2_insert_into_dir(sb, dir, name, name_len, inode, type);

		return true;
	}
	else
	{
		return false;
	}	
}

void ext2_insert_into_dir(struct ext2_superblock *sb, struct ext2_inode *dir,
						  char *name, uint name_len, uint inode, uchar type)
{
	name_len = MIN(name_len, 255);
	const uint min_size = PAD(name_len + sizeof(struct ext2_dirent));
	const uint block_size = ext2_block_size(sb);

	if ((dir->mode & 0xf000) != EXT2_S_IFDIR)
		return;

	uchar buffer[block_size];
	uint i; // block #

	for (i = 0; i < dir->num_blocks; i++)
	{
		ext2_read_inode_block(sb, dir, buffer, i);

		struct ext2_dirent *ent = (void *)buffer;

		// While there are files in this block
		while ((uint)ent < (uint)(buffer + block_size))
		{
			// kprintf(" %d@%db,%d-%d", ent->inode, i, (uint)ent - (uint)buffer,
			// 		ent->rec_len);
			if (ent->inode == 0)
			{
				// This is the last item, just insert it at the end
				// TODO: check this actually fits!
				// TODO: if we are in a new block, actually create it!

				ent->rec_len = min_size;
				ent->name_len = name_len;
				memcpy(ent->name, name, name_len);
				ent->inode = inode;
				ent->file_type = type;

				kprintf(DEBUG
						"Inserted into dir (appending) at block=%d, b=%p \n",
						i, (uint)ent - (uint)buffer);

				goto finish;
			}

			uint this_min_size =
				PAD(ent->name_len + sizeof(struct ext2_dirent));
			uint available_size = ent->rec_len - this_min_size;

			// kprintf(",%d=%d/%d", ent->name_len, this_min_size,
			// available_size);

			if (available_size >= min_size)
			{
				// We can fit this in here
				struct ext2_dirent *inserting =
					(void *)(((uint)(void *)ent) + this_min_size);

				ent->rec_len = this_min_size;

				inserting->rec_len = available_size;
				inserting->name_len = name_len;
				inserting->inode = inode;
				inserting->file_type = type;
				memcpy(inserting->name, name, name_len);

				kprintf(DEBUG
						"Inserted into dir (splicing) at block=%d, b=%p \n",
						i, (uint)inserting - (uint)buffer);

				// Done!
				goto finish;
			}

			ent = (void *)(((uint)(void *)ent) + ent->rec_len);
		}
		// We ran out of files in this block, continue to the next one. This
		// works because files cannot span blocks
	}

	kprintf("\n");

	kprintf(WARN "Failed to insert!\n");

finish:
	kprintf("\n");
	ext2_write_inode_block(sb, dir, buffer, i);
	kprintf(DEBUG "[insert] writing inode block %d\n", i);
}

ssize_t ext2_read_inode(struct ext2_superblock *sb, struct ext2_inode *inode,
						void *buffer, ssize_t size)
{
	const uint block_size = ext2_block_size(sb);
	char transfer[block_size];

	uint fsize = MIN(inode->size, (uint)size);
	uint i;

	// Transfer full blocks straight to the output buffer
	for (i = 0; i < fsize / block_size; i++)
	{
		ext2_read_inode_block(sb, inode, buffer + i * block_size, i);
	}

	// If we have part of a block left over read it here first, then transfer
	// what we need
	if (i * block_size < fsize)
	{
		uint remainder = fsize % block_size;

		ext2_read_inode_block(sb, inode, transfer, i);
		memcpy(buffer + i * block_size, transfer, remainder);
	}

	return fsize;
}

static uint ext2_compute_absolute_block(struct ext2_superblock *sb,
										struct ext2_inode *inode, uint block)
{
	if (block >= 12)
	{
		kprintf(ERROR
				"Sorry, EXT2 can only access the first 12 (direct) blocks "
				"of an inode for now. Indirect look-up will be added later\n");
		kpanic("Invalid inode block");
	}

	return block;
}

bool ext2_read_inode_block(struct ext2_superblock *sb, struct ext2_inode *inode,
						   void *buffer, uint block)
{
	block = ext2_compute_absolute_block(sb, inode, block);
	uint block_address = inode->blocks[block];

	ext2_read_block(sb, buffer, block_address);

	return true;
}

bool ext2_write_inode_block(struct ext2_superblock *sb, struct ext2_inode *dir,
							void *buffer, uint block)
{
	block = ext2_compute_absolute_block(sb, dir, block);
	uint block_address = dir->blocks[block];

	kprintf(DEBUG "Writing size=%d, inode block %p, b=%d\n",
			ext2_block_size(sb), block_address * ext2_block_size(sb), block);

	ext2_write_block(sb, buffer, block_address);

	return true;
}

static uint ext2_bitmap_block(struct ext2_superblock *sb,
							  uint *bitmap_block, uint *index)
{
	const uint block_size = ext2_block_size(sb);

	while (*index > block_size)
	{
		*index -= block_size;
		*bitmap_block += 1;
	}

	return block_size;
}

bool ext2_check_in_bitmap(struct ext2_superblock *sb, uint bitmap_block,
						  uint index)
{
	const uint block_size = ext2_bitmap_block(sb, &bitmap_block, &index);

	uint byte = index / 8;
	uint bit = index % 8;

	uchar buffer[block_size];

	ext2_read_block(sb, buffer, bitmap_block);

	return !!(buffer[byte] & (1 << bit));
}

void ext2_set_in_bitmap(struct ext2_superblock *sb, uint bitmap_block,
						uint index, bool value)
{
	const uint block_size = ext2_bitmap_block(sb, &bitmap_block, &index);

	uint byte = index / 8;
	uint bit = index % 8;

	uchar buffer[block_size];

	ext2_read_block(sb, buffer, bitmap_block);

	uchar target_bit = 1 << bit;
	buffer[byte] =
		value ? (buffer[byte] | target_bit) : (buffer[byte] | ~target_bit);

	ext2_write_block(sb, buffer, bitmap_block);
}

uint ext2_first_zero_bit(struct ext2_superblock *sb, uint bitmap_block,
						 uint num_blocks, uint start_at)
{
	uint block_size = ext2_block_size(sb);

	for (uint block = bitmap_block; block < bitmap_block + num_blocks; block++)
	{
		// dword-array for performance
		uint buffer[block_size / 4];

		ext2_read_block(sb, buffer, block);

		// If this is the first block start at start_at, otherwise 0
		for (uint i = 0; i < block_size / 4; i++)
		{
			// The bitwise negative will be non-zero if there are zero bits in
			// the original.
			if (~buffer[i])
			{
				// 4 bytes * 8 bits * i dwords
				uint index =
					(4 * 8 * i) + (block - bitmap_block) * 8 * block_size;

				// __builtin_ffs gives us 1+the index of the least-significant 1
				// bit. Since we take the bitwise inverse this is actuall the
				// least significant 0 bit. This is a GCC intrinsic. This works
				// particularly well on little-endian systems where the least
				// significant bit happens to also correspond to the first bit
				// in the dword bitset.
				//
				// ________ ________ ________ ________
				//        ^ this is the LSB   ^
				//                            | this is the MSB
				//
				// This means that the LSB is also the first bit in the bitset.
				uint trailing = __builtin_ffs(~buffer[i]) - 1;

				return trailing + index;
			}
		}
	}

	return -1;
}

uint ext2_first_free_inode(struct ext2_superblock *sb)
{
	uint num_block_groups = ext2_num_block_groups(sb);

	kprintf(INFO "%d block groups\n", num_block_groups);

	for (uint bg_num = 0; bg_num < num_block_groups; bg_num++)
	{
		struct ext2_block_group_descriptor bgd =
			ext2_load_bgd(sb, 0);

		const uint block_size = ext2_block_size(sb);
		// + 1 because we need to round up (ie 1025 for 1024 size blocks will
		// yield 1, should 2)
		uint bitset_blocks = (sb->inodes_per_block_group / 8) / block_size + 1;

		// inodes start at 1
		uint inode =
			ext2_first_zero_bit(sb, bgd.inode_bitmap, bitset_blocks, 12) + 1;
		// This will overflow back to zero if no inode was found

		if (inode)
			return inode;
	}

	return 0;
}

// ^
// | this and | this are awfully similar, should refactor
//            v

uint ext2_first_free_block(struct ext2_superblock *sb)
{
	uint num_block_groups = ext2_num_block_groups(sb);

	for (uint bg_num = 0; bg_num < num_block_groups; bg_num++)
	{
		struct ext2_block_group_descriptor bgd =
			ext2_load_bgd(sb, 0);

		const uint block_size = ext2_block_size(sb);
		// + 1 because we need to round up (ie 1025 for 1024 size blocks will
		// yield 1, should 2)
		uint bitset_blocks = (sb->blocks_per_block_group / 8) / block_size + 1;

		// inodes start at 1
		uint block_no =
			ext2_first_zero_bit(sb, bgd.block_bitmap, bitset_blocks, 0);
		// This will overflow back to zero if no inode was found

		if (block_no != 0xffffffff)
			return block_no;
	}

	return 0;
}

struct ext2_dir_contains_data
{
	char *name;
	uint len;
	bool found;
};

static bool ext2_dir_contains_cb(uint inode, const char *name,
								uint name_len,
								struct ext2_dir_contains_data *d)
{
	if (strncmp((char *)name, d->name, MIN(name_len, d->len)) == 0)
	{
		d->found = true;
		return false;
	}

	return true;
}

bool ext2_dir_contains(struct ext2_superblock *sb, struct ext2_inode *dir,
					   char *name, uint len)
{
	if ((dir->mode & EXT2_F_TYPE) != EXT2_S_IFDIR)
	{
		return false;
	}

	struct ext2_dir_contains_data d =
		{
			.name = name,
			.len = len,
			.found = false,
		};

	ext2_dir_ls(sb, dir, ext2_dir_contains_cb, &d);

	return d.found;
}

struct ext2_dir_find_data
{
	char *name;
	uint len;
	uint inode;
};

bool ext2_dir_find_cb(uint inode, const char *name, uint len,
					  struct ext2_dir_find_data *d)
{
	if (strncmp((char *)name, d->name, MIN(len, d->len)) == 0)
	{
		d->inode = inode;
		return false;
	}

	return true;
}

uint ext2_dir_find(struct ext2_superblock *sb, struct ext2_inode *dir,
				   char *name, uint name_len)
{
	if ((dir->mode & EXT2_F_TYPE) != EXT2_S_IFDIR)
	{
		return false;
	}

	struct ext2_dir_find_data d =
		{
			.name = name,
			.len = name_len,
			.inode = 0,
		};

	ext2_dir_ls(sb, dir, ext2_dir_find_cb, &d);

	return d.inode;
}

uint ext2_alloc_new_block(struct ext2_superblock *sb,
						  uint block_group)
{
	struct ext2_block_group_descriptor bgd =
		ext2_load_bgd(sb, block_group);

	if (bgd.unallocated_blocks == 0)
		// TODO: handle out of blocks
		return ext2_alloc_new_block(sb, block_group + 1);

	// We can safely pass ~0 here as long as the FS is well formed
	// because we know there is at least 1 free block
	uint block = ext2_first_zero_bit(sb, bgd.block_bitmap, ~0, 0);

	ext2_set_in_bitmap(sb, bgd.block_bitmap, block, true);
	bgd.unallocated_blocks--;

	ext2_write_bgd(sb, block_group, &bgd);

	return block;
}
