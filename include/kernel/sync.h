#pragma once

#include <kint.h>

// Synchronization primitives

/// Spinlock
typedef int spinlock_t;

void sl_acquire(spinlock_t *sl);
void sl_release(spinlock_t *sl);
spinlock_t sl_new();

/// Semaphore
typedef int semaphore_t;

void sm_wait(semaphore_t sm);
void sm_signal(semaphore_t sm);
semaphore_t sm_new();

/// Initialize synchronization primitives, only call this once after the
/// scheduler has been initialized.
void init_sync();
