#pragma once

#include "kint.h"

/* Note: this requires virtual memory to be set up,
 * this WILL call malloc() */
void init_initrd_vfs(uchar *data);
