#pragma once

void _do_gc(unsigned int esp, unsigned int ebp);
void _mark(unsigned int value);
void _sweep();
