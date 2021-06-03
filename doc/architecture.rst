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

So far drivers must be written either using plain ``in`` and ``out``
instructions or on top of the existing PCI driver.

PCI Device Drivers
~~~~~~~~~~~~~~~~~~

PCI device drivers must register a ``struct pci_device_driver`` in order to
interface with a certain device (or class of devices). The relevant fields of
``struct pci_device_driver`` are shown here:

.. code-block::

    bool (* supports)(struct pci_device *dev);
    void (* init)(struct pci_device dev, uchar bus, uchar slot, uchar func);
    char *generic_name;

A PCI device driver must pass an instance of this structure to
``pci_register_device_driver`` (in ``include/kernel/dri/pci/pci.h``. If
``supports`` returns true, (for example, if the class and subclass of the
``struct pci_device`` are supported by teh driver) ``init`` will be called. At
this point the driver may do whatever it wishes with the PCI device, although
all blocking operations should be done in another thread (using ``spawn_thread``
in ``include/kernel/task.h`` for example).
