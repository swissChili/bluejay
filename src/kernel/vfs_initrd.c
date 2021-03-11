#include <initrd/initrd.h>
#include "kint.h"
#include "vfs.h"
#include "io.h"

struct fs_node real_initrd;
struct fs_node *initrd_content = NULL;

void init_initrd_vfs()
{
	memset(&real_initrd, 0, sizeof(real_initrd));

	real_initrd.flags = FS_DIRECTORY;
}
