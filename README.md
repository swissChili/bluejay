# Bluejay

<img src="share/branding/bluejay-unsplash.jpg" align="right" width="200">

Bluejay is a preemptive kernel for x86. It is inspired by modern day
UNIX-like systems and 80's Lisp machines. The goal is to create a
fully usable kernel and graphical Lisp environment.

While Bluejay is inspired by both UNIX and Lisp machines, it seeks to
replicate neither. No attempt is made to conform to POSIX or even
follow UNIX conventions. However, the "UNIX philosophy" of doing one
thing and doing it well is certainly a consideration.

*Photo by [Erin Minuskin](https://unsplash.com/@erinw) on Unsplash*

## Roadmap

- [x] Virtual memory
  - [x] Higher-half kernel
  - [x] Per-process memory 
  - [x] Placeholder physical allocator
  - [x] Efficient kernel virtual allocator
- [ ] Preemptive multitasking
  - [x] Multi-threading
  - [ ] Multi-process support
- [ ] Filesystem
  - [x] Virtual file system
  - [x] Initial ramdisk
  - [ ] Storage device drivers
    - [x] ATA PIO
    - [ ] SATA
  - [ ] Filesystem drivers
    - [ ] EXT2
    - [ ] FAT32
- [ ] System call API
  - [ ] Filesystem API
  - [ ] Memory management API (`sbrk`, `mmap`, etc)
  - [ ] Process/thread API (`spawn_process`, `spawn_thread`, etc)
- [ ] Lisp integrated into kernel
- [ ] User-space driver API
  - [ ] Lisp API
- [ ] Graphical subsystem
  - [ ] Graphical environment in Lisp
- [ ] Network stack in Lisp
  - [ ] Ethernet driver
  - [ ] IP
  - [ ] TCP
    - [ ] Graphical applications like browser, IRC client
  - [ ] UDP

## Documentation

Manual pages are available in `doc/`, and additional documentation is
available [on the wiki](https://wiki.swisschili.sh/Bluejay).
