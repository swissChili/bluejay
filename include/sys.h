#pragma once

// Bluejay System Calls

#define SYS_GIVEUP 0x100

/// Bluejay interrupt number
#define SYS_INT 0x80

/**
 * @brief Gives up the current threads remaining CPU time. Unless you have set
 * the thread to waiting, it will resume next time the scheduler decides it's
 * its turn. The behaviour is identical to if the thread had just run out of
 * time naturally.
 */
extern void sys_giveup();
