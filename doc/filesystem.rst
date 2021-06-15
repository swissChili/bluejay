Bluejay Filesystem
==================

Filesystem drivers are still a work in progress. To test a file system you will
want to create and mount a virtual block device. The makefile in ``src/kernel``
will generate an ``hd0.img`` EXT2 disk image for you automatically. The default
size is 32 megabytes, but you can create your own of any size if you want. Once
the image has been created it will be loaded by QEMU automatically.

In order to write to the virtual hard disk from your host operating system you
should mount it. The ``make mount`` command in ``src/kernel`` mount the image to
``$(BLUEJAY_ROOT)/mnt``. It will automatically give read and write permissions
to the current user. This operation uses ``sudo`` internally, you will be
prompted for your password.

Virtual Filesystem
------------------

The Bluejay VFS is heavily inspired by UNIX. It relies on inodes and a tree of
file nodes. The source can be found in ``src/kernel/vfs.c``. This also exports a
very low-level API for dealing with files -- including the usual read(),
write(), readdir(), etc -- but this should not be used for much longer. A high
level API utilizing file descriptors will be implemented to make this simpler.
