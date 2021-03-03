#include "vfs.h"

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

struct fs_dirent *fs_readdir(struct fs_node *node, uint index)
{
	if (!node || !node->vtable || !node->vtable->readdir || (node->flags & 7) != FS_DIRECTORY)
		return NULL;

	return node->vtable->readdir(node, index);
}

struct fs_node *fs_finddir(struct fs_node *node, char *name)
{
	if (!node || !node->vtable || !node->vtable->finddir || (node->flags & 7) != FS_DIRECTORY)
		return NULL;

	return node->vtable->finddir(node, name);
}
