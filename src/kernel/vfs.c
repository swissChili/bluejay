#include "vfs.h"
#include "io.h"

struct fs_node root, dev, initrd;
struct fs_vtable root_v, dev_v;

uint fs_read(struct fs_node *node, size_t offset, size_t size, uchar *buffer)
{
	if (!node || !node->vtable || !node->vtable->read)
		return 0;

	return node->vtable->read(node, offset, size, buffer);
}

uint fs_write(struct fs_node *node, size_t offset, size_t size, uchar *buffer)
{
	if (!node || !node->vtable || !node->vtable->write)
		return 0;

	return node->vtable->write(node, offset, size, buffer);
}

void fs_open(struct fs_node *node)
{
	if (!node || !node->vtable || !node->vtable->open)
		return;

	node->vtable->open(node);
}

void fs_close(struct fs_node *node)
{
	if (!node || !node->vtable || !node->vtable->close)
		return;

	node->vtable->close(node);
}

bool fs_readdir(struct fs_node *node, uint index, struct fs_dirent *out)
{
	if (!node || !node->vtable || !node->vtable->readdir ||
		(node->flags & 7) != FS_DIRECTORY)
		return false;

	return node->vtable->readdir(node, index, out);
}

struct fs_node *fs_finddir(struct fs_node *node, char *name)
{
	if (!node || !node->vtable || !node->vtable->finddir ||
		(node->flags & 7) != FS_DIRECTORY)
		return NULL;

	return node->vtable->finddir(node, name);
}

bool root_readdir(struct fs_node *node, uint index, struct fs_dirent *out)
{
	if (node == &root && index == 0)
	{
		// devfs
		memcpy(out->name, "dev", 4);
		out->inode = -1;

		return true;
	}
	else
		return false;
}

struct fs_node *root_finddir(struct fs_node *node, char *name)
{
	if (!strcmp(name, "dev"))
	{
		return &dev;
	}
	else return NULL;
}

bool dev_readdir(struct fs_node *node, uint index, struct fs_dirent *out)
{
	if (node == &dev && index == 0)
	{
		// initrd
		memcpy(out->name, "dirent", 7);
		out->inode = -1;
		return true;
	}
	else
		return false;
}

struct fs_node *dev_finddir(struct fs_node *node, char *name)
{
	if (node != &dev)
		return NULL;

	if (!strcmp(name, "initrd"))
	{
		return &initrd;
	}

	return NULL;
}

void init_vfs()
{
	memset(&root_v, 0, sizeof(root_v));
	memset(&dev_v, 0, sizeof(root_v));
	memset(&root, 0, sizeof(root));
	memset(&dev, 0, sizeof(dev));

	root.flags = FS_DIRECTORY;
	root.vtable = &root_v;
	dev.flags = FS_DIRECTORY;
	dev.vtable = &dev_v;
	initrd.flags = FS_DIRECTORY;

	root_v.readdir = root_readdir;
	root_v.finddir = root_finddir;

	dev_v.readdir = dev_readdir;
	dev_v.finddir = dev_finddir;
}

uint fs_mount(struct fs_node *target, struct fs_node *source)
{
	if (target->flags ^ FS_DIRECTORY)
		return 1; // target is not a directory

	if (target->flags & FS_MOUNT)
		return 2; // already mounted

	if (source->flags ^ FS_DIRECTORY)
		return 3; // source is not a directory

	target->flags |= FS_MOUNT;
	target->mount = source;

	return 0;
}
