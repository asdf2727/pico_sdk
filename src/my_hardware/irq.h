#pragma once

#include <hardware/structs/io_bank0.h>

#define IRQ_PIN_BITS(val, pin) ((val) << (((pin) & 7) * 4))

#define IRQ_STATUS(pin, bits) (io_bank0_hw->proc0_irq_ctrl.ints[(pin) >> 3] & IRQ_PIN_BITS(bits, pin))
#define IRQ_CLEAR(pin, bits) io_bank0_hw->intr[(pin) >> 3] = IRQ_PIN_BITS(bits, pin)

#define IRQ_ENABLE(pin, bits) IRQ_CLEAR(pin, bits); io_bank0_hw->proc0_irq_ctrl.inte[(pin) >> 3] |= IRQ_PIN_BITS(bits, pin)
#define IRQ_DISABLE(pin) io_bank0_hw->proc0_irq_ctrl.inte[(pin) >> 3] &= ~IRQ_PIN_BITS(0xf, pin)
