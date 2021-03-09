#pragma once

#include "kint.h"

struct fs_vtable;

struct fs_node
{
	char name[128]; /* file name */
	uint inode;     /* identifier */
	uint flags;     /* type of node */
	uint mask;      /* permissions */
	uint gid;       /* group id */
	uint uid;       /* user id */
	size_t size;    /* size in bytes */
	uint dri_res;   /* reserved for driver */

	struct fs_vtable *vtable;

	struct fs_node *mount; /* used for mounts */
};

struct fs_dirent
{
	char name[128];
	uint inode;
};

typedef uint (* fs_read_t)(struct fs_node *node, size_t offset, size_t size, uchar *buffer);
typedef uint (* fs_write_t)(struct fs_node *node, size_t offset, size_t size, uchar *buffer);
typedef void (* fs_open_t)(struct fs_node *node);
typedef void (* fs_close_t)(struct fs_node *node);

typedef uint (* fs_readdir_t)(struct fs_node *node, uint index, struct fs_dirent *dirent);
typedef uint (* fs_finddir_t)(struct fs_node *node, char *name, struct fs_node *out);

struct fs_vtable
{
	fs_read_t read;
	fs_write_t write;
	fs_open_t open;
	fs_close_t close;
	fs_readdir_t readdir;
	fs_finddir_t finddir;
};

enum fs_flags
{
	FS_FILE = 1,
	FS_DIRECTORY,
	FS_CHARDEVICE,
	FS_BLOCKDEVICE,
	FS_PIPE,
	FS_SYMLINK,
	
	FS_MOUNT = 8,   /* Can be or'd with others */
};

extern struct fs_node root, dev, initrd;

/* Not to be confused normal open, close, etc functions, these operate
 * on the VFS directly
 * read and write return the number of bytes written/read,  */

uint fs_read(struct fs_node *node, size_t offset, size_t size, uchar *buffer);
uint fs_write(struct fs_node *node, size_t offset, size_t size, uchar *buffer);
void fs_open(struct fs_node *node);
void fs_close(struct fs_node *node);

/* For all of the functions that return integers;
 * 0 = okay
 * non-zero = error */

uint fs_readdir(struct fs_node *node, uint index, struct fs_dirent *out);
uint fs_finddir(struct fs_node *node, char *name, struct fs_node *out);

/* Returns the following error codes:
 * 0: success
 * 1: target is not a directory
 * 2: target is already a mount point
 * 3: source is not a directory */
uint fs_mount(struct fs_node *target, struct fs_node *source);

void init_vfs();
