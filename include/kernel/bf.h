#pragma once

#include <kint.h>

struct binfmt
{
	bool (*valid)(char header[16]);
	void (*load)(uchar *buffer, uint len);
};
