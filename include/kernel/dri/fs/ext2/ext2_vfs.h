#pragma once

#include "ext2.h"
#include <vfs.h>

// VFS specific EXT2 functions

/// ext2 inode -> vfs node
struct fs_node *ext2_inode2vfs(struct ext2_superblock *sb, uint inode,
							   char *name, uint name_len);
