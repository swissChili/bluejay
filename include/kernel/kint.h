#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef unsigned long size_t;

typedef signed long ssize_t;

typedef _Bool bool;

enum
{
	false = 0,
	true,
};

#define NULL 0
#define MIN(a, b) ((a)>(b)?(b):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))

/// Pads num to an integer size boundary
#define PAD(num) ((num + 3) & (~0b11))

/// Perform integer division and round up
#define IDIV_CEIL(num, den) (((num) + ((den) - 1)) / (den))

// Coerce into 1 or 0
#define BOOL(a) (!(!(a)))
