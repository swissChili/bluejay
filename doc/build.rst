Building Bluejay
================

Bluejay uses the home-grown ``Jmk`` build system, which is basically just a GNU
m4 script that generates makefiles from ``Jmk`` files --- makefiles with some
custom macros.

To build a fresh clone of Bluejay the first thing you will need to do is run
``bin/jmk`` to generate your makefiles for you. You should get some output like
this:

.. code-block::

    Processing ./boot/initrd/Jmk
    Processing ./src/kernel/dri/ata_pio/Jmk
    Processing ./src/kernel/dri/ahci/Jmk
    Processing ./src/kernel/dri/pci/Jmk
    Processing ./src/kernel/Jmk
    Processing ./src/mkinitrd/Jmk
    Processing ./src/lisp/Jmk

Then just build using ``src/kernel/Makefile``. There are a few additional
targets for your convenience:

- ``qemu`` builds and launches the kernel using QEMU's SeaBIOS
- ``qemu-iso`` builds a GRUB ISO and launches using QEMU
- ``install`` builds a GRUB ISO and installs it to ``boot/bluejay.iso``
- ``debug`` launches kernel in QEMU and launches GDB in the terminal.
- ``debug-wait`` launches kernel in QEMU and starts a GDB server on ``localhost:1234``.
  This is recommended if you want to debug since you can connect to it from vscode or
  any other IDE. ``.vscode/launch.json`` is set up to work with this so you can debug
  the kernel very easily.

In order to build Bluejay you will need the following dependencies::

    gcc gcc-multilib nasm qemu-system-i386 make m4 python3 awk

There are some additional dependencies for building a GRUB ISO but I don't
remember them at the time of writing.

Common Issues
-------------

If when launching Bluejay in QEMU with ``make qemu`` you see the message ::

    [ DEBUG ] mb.mods_addr = <whatever>, 0x<whatever>

Followed by a page fault (``#PF``), your QEMU is out of date. You can either run
the GRUB ISO (with ``make qemu-iso``), which is slower, or upgrade your QEMU to
at least version 6.0.

This is because prior to 6.0 QEMU's integrated bootloader did not support
multiboot modules. This message is caused by the kernel attempting to read a
module that was loaded incorrectly.
