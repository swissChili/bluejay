# Bluejay

<img src="share/branding/bluejay-unsplash.jpg" align="right" width="200">

[![builds.sr.ht status](https://builds.sr.ht/~swisschili/bluejay/commits/.build.yml.svg)](https://builds.sr.ht/~swisschili/bluejay/commits/.build.yml?)

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
  - [ ] Multi-process support (in progress)
- [ ] Device drivers
  - [x] PCI
  - [ ] USB
    - [ ] Mouse + keyboard drivers
  - [ ] Storage device drivers
    - [x] ATA PIO
    - [ ] SATA
- [x] Synchronization primitives
  - [x] Process-local spin lock
  - [x] Kernel-level semaphore
- [ ] Filesystem
  - [x] Virtual file system
  - [x] Initial ramdisk
  - [ ] Filesystem drivers
    - [ ] EXT2 (in progress)
      - [x] Read
      - [ ] Write
      - [ ] Consistency check (`fsck`)
    - [ ] FAT32
- [ ] System call API
  - [ ] Filesystem API
  - [ ] Memory management API (`sbrk`, `mmap`, etc)
  - [ ] Process/thread API (`spawn_process`, `spawn_thread`, etc)
- [ ] Lisp compiler
  - [x] JIT compiler using dynasm
    - [x] Basic compilation
    - [x] GC
    - [x] Lexical closures
  - [ ] Standard library (in progress)
    - [ ] CLOS-style OO library
- [ ] Kernel module API
  - [ ] Lisp API
- [ ] Graphical subsystem
  - [ ] Graphical environment
- [ ] Network stack
  - [ ] Ethernet driver
  - [ ] IP
  - [ ] TCP
    - [ ] Graphical applications like browser, IRC client
  - [ ] UDP

## Documentation

The [Bluejay manual](https://bluejay.readthedocs.io) contains out dated
documentation. A complete manual will be available before the 1.0 release. For
now there isn't a ton to document as the user-space API is non-existent.
