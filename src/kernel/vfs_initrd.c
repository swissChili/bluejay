#include "alloc.h"
#include "io.h"
#include "kint.h"
#include "log.h"
#include "vfs.h"
#include <initrd/initrd.h>

struct initrd_fs_entry
{
	char *name;
	uchar *data;
	uint size;
};

static struct fs_node real_initrd;
static struct fs_node *initrd_content = NULL;
static struct initrd_fs_entry *entries = NULL;
static struct fs_vtable initrd_v;
static uint num_initrd_files = 0;

bool initrd_readdir(struct fs_node *node, uint i, struct fs_dirent *out)
{
	if (node != &real_initrd)
		return false;

	if (i >= num_initrd_files)
		return false;

	memcpy(out->name, entries[i].name, strlen(entries[i].name) + 1);
	out->inode = i;
	return true;
}

struct fs_node *initrd_finddir(struct fs_node *node, char *name)
{
	if (node != &real_initrd)
		return NULL;

	for (int i = 0; i < num_initrd_files; i++)
	{
		if (strcmp(entries[i].name, name) == 0)
			return &initrd_content[i];
	}
	return NULL;
}

void initrd_open(struct fs_node *node)
{
}

void initrd_close(struct fs_node *node)
{
}

uint initrd_read(struct fs_node *node, size_t offset, size_t size, uchar *buffer)
{
	if (offset > node->size)
		return 0;

	size = MIN(size, node->size - offset);
	memcpy(buffer, entries[node->dri_res].data, size);

	return size;
}

void init_initrd_vfs(uchar *data)
{
	memset(&real_initrd, 0, sizeof(real_initrd));

	real_initrd.flags = FS_DIRECTORY;
	real_initrd.vtable = &initrd_v;

	initrd_v.readdir = initrd_readdir;
	initrd_v.finddir = initrd_finddir;
	initrd_v.open = initrd_open;
	initrd_v.close = initrd_close;
	initrd_v.read = initrd_read;

	// parse initrd
	struct initrd_global_header *h = (struct initrd_global_header *)data;

	if (h->magic != INITRD_MAGIC)
	{
		kprintf("initrd magic is wrong: %x should be %x\n", h->magic,
				INITRD_MAGIC);
		kpanic("initrd magic");
	}

	num_initrd_files = h->num_files;
	initrd_content = malloc(sizeof(struct fs_node) * num_initrd_files);
	entries = malloc(sizeof(struct initrd_fs_entry) * num_initrd_files);

	// create fs_nodes for files
	struct initrd_file_header *f =
		(struct initrd_file_header *)(data +
									  sizeof(struct initrd_global_header));

	for (int i = 0; i < num_initrd_files; i++)
	{
		uchar *data = (uchar *)((size_t)f + sizeof(struct initrd_file_header));

		entries[i].data = data;
		entries[i].name = f->name;
		entries[i].size = f->size;

		initrd_content[i] = (struct fs_node){
			.dri_res = i,
			.flags = FS_FILE,
			.mask = 0b101001001,
			.uid = 0,
			.gid = 0,
			.size = f->size,
			.vtable = &initrd_v,
			.mount = NULL
		};

		memcpy(initrd_content[i].name, f->name, strlen(f->name));

		f = (struct initrd_file_header *)((size_t)data + f->size);
	}

	// mount initrd to /dev/initrd/
	if (fs_mount(&initrd, &real_initrd))
		kpanic("Failed to mount initrd to /dev/initrd");
}
