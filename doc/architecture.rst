Architecture
============

This document seeks to provide a brief overview of Bluejay architecture. This
should be a good starting point for understanding the code.

Bluejay is exclusively a multiboot kernel, it neither provides nor supports
alternative bootloaders.

The bootloader (probably GRUB) will initially run the code in ``boot.s``. This
is where it all begins. This code sets up segmentation and paging and maps the
higher-half of virtual memory (everything above ``0xC0000000``) to the kernel. 
At first it only maps 8 megabytes, more memory can be mapped on request.

After moving to high memory the kernel jumps to C code and enters ``kmain`` in
``main.c``. This is the highest level procedure in the kernel, which sets up
kernel services and drivers one at a time.

This includes VGA, keyboard, and PCI drivers, as well as paging and preemptive
multi tasking.

Multi tasking
-------------

Multi tasking is handled by code in ``task.c``. It is first initialized in
``init_tasks``, which sets up the initial task. Once this is called kernel
threads can be spawned at will.

Every clock tick an interrupt is triggered (see ``clock.c`` for timing) which
causes a task switch to occur. Bluejay uses a simple round-robin scheduler, and
there is no way for tasks to voluntarily give up their processing time (even in
the case of blocking IO operations). ``task.c`` contains the implementation of
the scheduler.

Drivers
-------

So far I have only written very low level drivers (stuff like ATA PIO, PCI, VGA
text mode, etc). These drivers have all been "bare-metal", ie: interfacing with
hardware through ``in`` and ``out`` instructions. Higher level drivers will be
built on top of existing ones. An interface will be created for defining, for
example, PCI device drivers, or USB device drivers.
