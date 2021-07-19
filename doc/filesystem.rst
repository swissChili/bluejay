Bluejay Filesystem
==================

Filesystem drivers are still a work in progress. To test a file system you will
want to create and mount a virtual block device. The makefile in ``src/kernel``
will generate an ``hd0_ext2.img`` EXT2 disk image for you automatically. The
default size is 32 megabytes, but you can create your own of any size if you
want. Once the image has been created it will be loaded by QEMU automatically.

In order to write to the virtual hard disk from your host operating system you
should mount it. The ``make mount`` command in ``src/kernel`` mount the image to
``$(BLUEJAY_ROOT)/mnt``. If you are using an EXT2 filesystem you should probably
change the owner of that directory once it is mounted so that you can write to
it.

Virtual Filesystem
------------------

The Bluejay VFS is heavily inspired by UNIX. It relies on inodes and a tree of
file nodes. The source can be found in ``src/kernel/vfs.c``. This also exports a
very low-level API for dealing with files -- including the usual read(),
write(), readdir(), etc -- but this should not be used for much longer. A high
level API utilizing file descriptors will be implemented to make this simpler.

Filesystem Drivers
------------------

The current filesystem driver(s) available in Bluejay are:

- ``ext2``
    - Read-only support, write support is in progress

Creating a Virtual Drive in QEMU
--------------------------------

By default ``make qemu`` will load ``hd0_$(FS).img`` as the virtual hard drive
for Bluejay. ``FS`` defaults to ``ext2`` but can be set in your ``Jmk.options``
to any value. If this file does not exist it will be created using
``mkfs.$(FS)``, ie ``mkfs.ext2`` by default. The default size of the file system
is 35 megabytes, although you can create one of any size manually if you want.
35 megabytes is plenty for testing though.

The ``make mount`` command will mount the current virtual hard drive in
``$(ROOT)/mnt`` (where ``$(ROOT)`` is the root directory of the Bluejay sources,
not ``/``). This command requires superuser privileges. If you want to give your
(host) user account write permissions use ``chown -R user:group /path/to/mnt``
where ``user`` and ``group`` are the user and group you want to own the files.

Currently Bluejay ignores file permissions so it doesn't matter who you set the
owner to.
