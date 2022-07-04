#pragma once

#include <vfs.h>

struct ext2_superblock
{
	/// Total number of inodes
	uint total_inodes;

	/// Total number of blocks
	uint total_blocks;

	/// Number of blocks reserved for superuser
	uint blocks_reserved_for_superuser;

	/// Number of unallocated blocks
	uint unallocated_blocks;

	/// Number of unallocated inodes
	uint unallocated_inodes;

	/// Block number containing the superblock
	uint superblock_block_number;

	/// the number to shift 1024 to the left by to get the block size
	uint block_size_shift;

	/// the number to shift 1024 to the left by to get the fragment size
	uint fragment_size_shift;

	/// Number of blocks in one block group
	uint blocks_per_block_group;

	/// Number of fragments in one block group
	uint fragments_per_block_group;

	/// Number of inodse in one block group
	uint inodes_per_block_group;

	/// UNIX time of last mount
	uint last_mount_time;

	/// UNIX time of last write
	uint last_write_time;

	/// Number of mounts since last consistency check
	ushort mounts_since_last_fsck;

	/// Number of mounts allowed between consistency checks
	ushort mounts_allowed_before_fsck;

	/// EXT2 signature, should be 0xef53
	ushort signature;

	/// File system state, see enum ext2_fs_state
	ushort state;

	/// What to do in case of an error, see enum ext2_error_case
	ushort error_case;

	/// Minor portion of version
	ushort version_minor;

	/// UNIX time of last consistency check
	uint last_fsck_time;

	/// Max time between consistency checks
	uint fsck_time_interval;

	/// Operating system ID of creator
	uint creator_os_id;

	/// Major portion of version
	uint version_major;

	/// User ID that can use reserved blocks
	ushort reserved_blocks_uid;

	/// Group ID that can use reserved blocks
	ushort reserved_blocks_gid;
};

enum ext2_fs_state
{
	EXT2_STATE_CLEAN = 1,
	EXT2_STATE_ERRORS = 2,
};

enum ext2_error_case
{
	EXT2_ERROR_IGNORE = 1,
	EXT2_ERROR_REMOUNT = 2,
	EXT2_ERROR_KPANIC = 3,
};

enum ext2_os_id
{
	EXT2_OS_LINUX = 0,
	EXT2_OS_HURD,
	EXT2_OS_MASIX,
	EXT2_OS_FREEBSD,
	EXT2_OS_OTHER_BSD,
};

#define EXT2_SIGNATURE 0xef53

struct ext2_block_group_descriptor
{
	/// Address of block usage bitmap
	uint block_bitmap; // 4

	/// Address of inode usage bitmap
	uint inode_bitmap; // 8

	/// Starting block address of inode table
	uint inode_table_start_block; // 12

	/// Number of unallocated blocks in this group
	ushort unallocated_blocks; // 14

	/// Number of unallocated inodes in this group
	ushort unallocated_inodes; // 16

	/// Number of directories in this group
	ushort num_dirs; // 18

	ushort padding; // 20

	uchar reserved[12]; // 32
};

struct ext2_inode
{
	/// The format and permissions of the file, see EXT2_S_*
	ushort mode;

	/// The user id of the owner
	ushort uid;

	/// The lower 32 bits of the file size
	uint size;

	/// Last time this was accessed in UNIX time
	uint access_time;

	/// Time this was created in UNIX time
	uint created_time;

	/// Time this was last modified in UNIX time
	uint modified_time;

	/// Time this was deleted in UNIX time
	uint deleted_time;

	/// Owner group id
	ushort gid;

	/// Reference count for hard links
	ushort links_count;

	/// Number of 512 byte blocks storing this files data, NOT EXT2 blocks!
	uint num_blocks;

	/// Flags describing how this file can be accessed
	uint flags;

	/// First field reserved for the operating system
	uint os_reserved_1;

	union
	{
		/// The blocks of this file
		uint blocks[15];

		struct
		{
			/// The direct blocks
			uint direct_blocks[12];

			/// The single indirect block
			uint ind_block;

			/// The double indirect block
			uint ind2_block;

			/// The triple indirect block
			uint ind3_block;
		};
	};

	/// Used by NFS to version files
	uint generation;

	/// File attributes, unused
	uint file_acl;

	/// Directory attributes, unused
	uint dir_acl;

	/// Location of file fragment, unused
	uint faddr;

	/// Second field reserved for the operating system
	uchar os_reserved_2[12];
};

enum
{
	// File types
	EXT2_S_IFSOCK = 0xc000,
	EXT2_S_IFLINK = 0xa000,
	EXT2_S_IFREG = 0x8000,
	EXT2_S_IFBLK = 0x6000,
	EXT2_S_IFDIR = 0x4000,
	EXT2_S_IFCHR = 0x2000,
	EXT2_S_IFIFO = 0x1000,

	/// Set UID
	EXT2_S_ISUID = 0x0800,
	/// Set GUID
	EXT2_S_ISGID = 0x0400,
	/// Sticky
	EXT2_S_ISVTX = 0x0200,

	// Normal UNIX file permissions
	EXT2_S_IRUSR = 0x0100,
	EXT2_S_IWUSR = 0x0080,
	EXT2_S_IXUSR = 0x0040,
	EXT2_S_IRGRP = 0x0020,
	EXT2_S_IWGRP = 0x0010,
	EXT2_S_IXGRP = 0x0008,
	EXT2_S_IROTH = 0x0004,
	EXT2_S_IWOTH = 0x0002,
	EXT2_S_IXOTH = 0x0001,
};

/// File type flag (used on inode->mode)
#define EXT2_F_TYPE 0xf000

extern const uchar ext2_s_to_ft[];

/// Converts the file "mode" (inode->mode, EXT2_S_*) to a file type
/// (EXT2_FT_*).
#define EXT2_S_TO_FT(s) (ext2_s_to_ft[s >> 12])

struct ext2_dirent
{
	uint inode;
	ushort rec_len;
	uchar name_len;
	uchar file_type;
	char name[];
};

enum
{
	EXT2_FT_UNKNOWN = 0,
	EXT2_FT_REGULAR_FILE,
	EXT2_FT_DIR,
	EXT2_FT_CHRDEV,
	EXT2_FT_BLKDEV,
	EXT2_FT_FIFO,
	EXT2_FT_SOCK,
	EXT2_FT_SYMLINK,
};

inline uint ext2_block_size(struct ext2_superblock *sb)
{
	return 1024 << sb->block_size_shift;
}

/// Read a file system block (0-indexed), if necessary multiple disk
/// blocks will be read automatically
void ext2_read_block(struct ext2_superblock *sb, void *buffer,
					 uint block);
void ext2_write_block(struct ext2_superblock *sb, void *buffer,
					  uint block);

struct ext2_superblock ext2_read_superblock();

void ext2_write_superblock(struct ext2_superblock *sb);

/// Just for fun, corrupt the superblock (obviously don't actually do
/// this)
void ext2_corrupt_superblock_for_fun();

void ext2_mount(struct fs_node *where);

bool ext2_valid_filesystem();

/**
 * @brief Get or set an inode
 * @param inode Either a pointer to the value to be read (set=false),
 * or the value to be written (set=true).
 * @param set True if the inode should be set (written), false
 * otherwise.
 */
bool ext2_get_or_set_inode(struct ext2_superblock *sb, uint number,
						   struct ext2_inode *inode, bool set);

bool ext2_find_inode(struct ext2_superblock *sb, uint number,
					 struct ext2_inode *inode);

bool ext2_set_inode(struct ext2_superblock *sb, uint number,
					struct ext2_inode *inode);

/// Load a block group descriptor for a certain block group
struct ext2_block_group_descriptor ext2_load_bgd(
	struct ext2_superblock *sb, uint block_group);

/// Write a block group descriptor for a certain block group
void ext2_write_bgd(struct ext2_superblock *sb, uint block_group,
					struct ext2_block_group_descriptor *d);

/// Load or write a BGD
void ext2_load_or_write_bgd(struct ext2_superblock *sb,
							uint block_group,
							struct ext2_block_group_descriptor *d,
							bool set);

/// List the contents of a directory dir. Calls `cb` for each item. If
/// `dir` is not a directory, returns false. Otherwise returns true.
/// if cb returns true, ls will continue. Otherwise it will stop.
bool ext2_dir_ls(struct ext2_superblock *sb,
				 struct ext2_inode *dir,
				 bool (*cb)(uint inode,
							const char *name,
							uint name_len,
							void *data),
				 void *data);

/// Read a data block for the inode, 0 being the first block of the file.
bool ext2_read_inode_block(struct ext2_superblock *sb,
						   struct ext2_inode *inode,
						   void *buffer,
						   uint block);

/// Write a data block for the inode, 0 being the first block of the file.
bool ext2_write_inode_block(struct ext2_superblock *sb, struct ext2_inode *dir,
							void *buffer, uint block);

ssize_t ext2_read_inode(struct ext2_superblock *sb,
						struct ext2_inode *inode, void *buffer,
						ssize_t size);

/**
 * @brief Set a block in a bitmap
 * @param bitmap_block The first block of the bitmap, not necessarily the block
 * that this specific bit is in.
 * @param index The bit to check, starting at 0
 * @returns The value of the bit
 */
bool ext2_check_in_bitmap(struct ext2_superblock *sb, uint bitmap_block,
						  uint index);

/**
 * @brief Set a block in a bitmap
 * @param bitmap_block The first block of the bitmap, not necessarily the block
 * that this specific bit is in.
 * @param index The bit to set, starting at 0
 * @param value The value of the bit, 0 for 0, non-0 for 1
 */
void ext2_set_in_bitmap(struct ext2_superblock *sb, uint bitmap_block,
						uint index, bool value);

/**
 * @brief Find the first zero bit in a bitset
 * @param bitmap_block The first block of the bitmap
 * @param num_blocks The number of blocks in the bitmap, or ~0 for no limit
 * (UNSAFE).
 * @param start_at The first bit to search from, useful if you want to ignore
 * the first reserved inodes.
 * @returns The index of the first zero bit, or -1 if there is none
 */
uint ext2_first_zero_bit(struct ext2_superblock *sb, uint bitmap_block,
						 uint num_blocks, uint start_at);

/**
 * Find the first free inode number.
 * @returns The inode if it was found, 0 if there are no free inodes.
 */
uint ext2_first_free_inode(struct ext2_superblock *sb);

void ext2_insert_into_dir(struct ext2_superblock *sb, struct ext2_inode *dir,
						  char *name, uint name_len, uint inode, uchar type);

/**
 * Creates a hard link
 * @param dir Directory to create the hard link in
 * @param name Name of the link  to create
 * @param name_len length of name, not including (optional) NULL byte
 * @param inode Inode number of link target
 */
bool ext2_hard_link(struct ext2_superblock *sb, struct ext2_inode *dir,
                    char *name, uint name_len, uint inode);

/**
 * @brief Does a directory contain a certain entry?
 * @param sb Superblock
 * @param dir The directory to search in
 * @param name The name of the entry to look for
 * @param len The length of the name, not including the optional NULL byte
 * @returns True if the entry is found, false if not, or if something
 * goes wrong (ie: dir is not a directory)
 */
bool ext2_dir_contains(struct ext2_superblock *sb, struct ext2_inode *dir,
					   char *name, uint len);

/**
 * @brief Find an entry in a directory
 * @param dir Directory to search in
 * @param name Name of file
 * @param name_len Length of name, not including (optional) NULL byte.
 * @returns 0 if no entry could be found, otherwise inode number for
 * that entry
 */
uint ext2_dir_find(struct ext2_superblock *sb, struct ext2_inode *dir,
				   char *name, uint name_len);
