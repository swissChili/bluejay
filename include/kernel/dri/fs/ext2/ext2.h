#pragma once

#include <vfs.h>

struct ext2_superblock
{
	// Total number of inodes
	uint total_inodes;
	// Total number of blocks
	uint total_blocks;
	// Number of blocks reserved for superuser
	uint blocks_reserved_for_superuser;
	// Number of unallocated blocks
	uint unallocated_blocks;
	// Number of unallocated inodes
	uint unallocated_inodes;
	// Block number containing the superblock
	uint superblock_block_number;
	// the number to shift 1024 to the left by to get the block size
	uint block_size_shift;
	// the number to shift 1024 to the left by to get the fragment size
	uint fragment_size_shift;
	// Number of blocks in one block group
	uint blocks_per_block_group;
	// Number of fragments in one block group
	uint fragments_per_block_group;
	// Number of inodse in one block group
	uint inodes_per_block_group;
	// UNIX time of last mount
	uint last_mount_time;
	// UNIX time of last write
	uint last_write_time;
	// Number of mounts since last consistency check
	ushort mounts_since_last_fsck;
	// Number of mounts allowed between consistency checks
	ushort mounts_allowed_before_fsck;
	// EXT2 signature, should be 0xef53
	ushort signature;
	// File system state, see enum ext2_fs_state
	ushort state;
	// What to do in case of an error, see enum ext2_error_case
	ushort error_case;
	// Minor portion of version
	ushort version_minor;
	// UNIX time of last consistency check
	uint last_fsck_time;
	// Max time between consistency checks
	uint fsck_time_interval;
	// Operating system ID of creator
	uint creator_os_id;
	// Major portion of version
	uint version_major;
	// User ID that can use reserved blocks
	ushort reserved_blocks_uid;
	// Group ID that can use reserved blocks
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

void ext2_mount(struct fs_node *where);
