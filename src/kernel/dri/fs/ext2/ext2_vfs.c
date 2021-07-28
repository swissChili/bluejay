#include <dri/fs/ext2/ext2.h>
#include <dri/fs/ext2/ext2_vfs.h>
#include <sync.h>
#include <alloc.h>
#include <log.h>
#include <io.h>

struct ext2_fs_dirent
{
	struct fs_node *node;
	char name[256];
	uint name_len;
	uint inode;

	struct ext2_fs_dirent *next;
};

struct ext2_fs_data
{
	struct ext2_superblock *sb;

	/// Cached inode
	struct ext2_inode *in;

	/// Reference count for the inode cache. Once this hits 0 we free
	/// in. Only used for **files**, directories **always** cache
	/// their inode.
	uint rc;

	semaphore_t lock;

	/// Entries (if this is a directory)
	struct ext2_fs_dirent *dirent;
};

uint ext2_file_read(struct fs_node *node, size_t ofs, size_t size,
					uchar *buffer)
{
	struct ext2_fs_data *d = node->dri_res_p;
	const uint block_size = ext2_block_size(d->sb);

	// temporary buffer
	uchar *tmp = malloc(block_size);

	uint num_read = 0;

	if (!buffer)
	{
		kprintf(WARN "ext2_file_read out of memory!\n");
		return 0;
	}

	uint block_start = ofs / block_size;
	uint block_offset = ofs % block_size;
	int to_read;

	do
	{
		// How much from this block to read
		to_read = MIN(block_size - block_offset, size);
		ext2_read_inode_block(d->sb, d->in, tmp, block_start);

		// Copy that to the user buffer
		memcpy(buffer, tmp + block_offset, to_read);
		buffer += to_read;
		size -= to_read;
		num_read += to_read;

		// Move to the next block
		block_start++;
		block_offset = 0;
	}
	while (to_read > 0);

	free(tmp);
	return num_read;
}

void ext2_file_open(struct fs_node *node)
{
	struct ext2_fs_data *d = node->dri_res_p;

	sm_wait(d->lock);

	if (d->rc++ == 0)
	{
		d->in = malloc(sizeof(struct ext2_inode));

		if (d->in == NULL)
		{
			// lol
			kpanic("ext2_file_open: ENOMEM");
		}

		if (!ext2_find_inode(d->sb, node->inode, d->in))
		{
			kprintf(ERROR "ext2_file_open: can't find inode %d\n",
					node->inode);

			free(d->in);
			goto done;
		}
	}

done:
	sm_signal(d->lock);
}

void ext2_file_close(struct fs_node *node)
{
	struct ext2_fs_data *d = node->dri_res_p;

	sm_wait(d->lock);

	if (--d->rc == 0)
	{
		free(d->in);
		d->in = NULL;
	}

	sm_signal(d->lock);
}

struct fs_vtable ext2_file_vfs_vt =
{
	.read = ext2_file_read,
	.write = NULL,
	.open = ext2_file_open,
	.close = ext2_file_close,
	.readdir = NULL,
	.finddir = NULL,
};

bool ext2_dir_readdir(struct fs_node *node, uint index, struct fs_dirent *dirent)
{
	struct ext2_fs_data *d = node->dri_res_p;
	struct ext2_fs_dirent *dent = d->dirent;

	uint i;

	for (i = 0; i < index && dent; i++, dent = dent->next)
	{
	}

	if (dent && i == index)
	{
		memcpy(dirent->name, dent->name, MAX(dent->name_len, FS_MAX_NAME_LEN));
		dirent->name_len = dent->name_len;
		dirent->inode = dent->inode;

		return true;
	}

	return false;
}

struct fs_node *ext2_dir_finddir(struct fs_node *node, char *name, uint name_len)
{
	name_len = MIN(name_len, 256);
	struct ext2_fs_data *d = node->dri_res_p;
	struct ext2_fs_dirent *dent = d->dirent;

	for (; dent; dent = dent->next)
	{
		if (strncmp(dent->name, name, name_len) == 0)
		{
			return dent->node;
		}
	}

	return NULL;
}

struct fs_vtable ext2_dir_vfs_vt =
{
	.read = NULL,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.readdir = ext2_dir_readdir,
	.finddir = ext2_dir_finddir,
};

struct ext2_fs_dirent_to_fs_data
{
	struct ext2_fs_data *d;
	struct ext2_fs_dirent *last;
	struct ext2_superblock *sb;
};

static bool ext2_dirent_to_fs_node_cb(uint inode, const char *name,
							   uint name_len, void *data)
{
	struct ext2_fs_dirent_to_fs_data *d = data;

	struct ext2_fs_dirent *dent =
		malloc(sizeof(struct ext2_fs_dirent));

	if (strncmp(".", (char *)name, name_len) == 0 ||
		strncmp("..", (char *)name, name_len) == 0)
	{
		return true;
	}

	dent->node = ext2_inode2vfs(d->sb, inode, (char *)name, name_len);
	dent->name_len = name_len;
	memcpy(dent->name, name, MIN(name_len, 256));
	dent->inode = inode;

	if (d->last)
	{
		d->last->next = dent;
		d->last = dent;
	}
	else
	{
		d->last = d->d->dirent = dent;
	}

	return true;
}

struct fs_node *ext2_inode2vfs(struct ext2_superblock *sb, uint inode,
							   char *name, uint name_len)
{
	struct ext2_inode in;

	if (!ext2_find_inode(sb, inode, &in))
	{
		// Something has gone terribly wrong!
		return NULL;
	}

	struct ext2_fs_data *d = malloc(sizeof(struct ext2_fs_data));

	d->sb = sb;
	d->rc = 0;
	d->lock = sm_new();
	d->in = NULL;
	d->dirent = NULL;

	struct fs_node *n = malloc(sizeof(struct fs_node));

	n->name_len = MIN(name_len, FS_MAX_NAME_LEN);
	memcpy(n->name, name, n->name_len);
	n->inode = inode;
	n->dri_res_p = d;

	n->mask = in.mode & 0xfff;
	n->gid = in.gid;
	n->uid = in.uid;
	n->size = in.size;

	switch (in.mode & EXT2_F_TYPE)
	{
	case EXT2_S_IFREG:
	{
		n->flags = FS_FILE;
		n->vtable = &ext2_file_vfs_vt;

		break;
	}

	case EXT2_S_IFDIR:
	{
		n->flags = FS_DIRECTORY;
		n->vtable = &ext2_dir_vfs_vt;

		struct ext2_fs_dirent_to_fs_data data;
		data.d = d;
		data.last = NULL;
		data.sb = sb;

		ext2_dir_ls(sb, &in, ext2_dirent_to_fs_node_cb, &data);

		break;
	}

	default:
	{
		kprintf(ERROR "ext2_inode2vfs: unimplemented for dir type%d\n",
				EXT2_S_TO_FT(in.mode));
		kpanic("Unimplemented");
	}
	}

	return n;
}
