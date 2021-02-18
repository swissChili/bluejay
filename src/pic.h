#pragma once

#include "kint.h"
#include "registers.h"

#define PIC1 0x20
#define PIC2 0xa0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_EOI 0x20 // End of input

#define IRQ_TO_INT(irq) ((irq) + 32)

void pic_send_eoi(uchar interrupt);
void add_interrupt_handler(uchar interrupt, void (* handler)(struct registers));

void pic_remap();
