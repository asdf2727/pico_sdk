#pragma once

#define BIT(bit) (1ul << (bit))

#define REG_GET(reg, NAME) (((reg) & NAME##_BITS) >> NAME##_LSB)
#define REG_VAL(NAME, val) (((val) << NAME##_LSB) & NAME##_BITS)
#define REG_RW_SET(reg, mask, val) (reg) = ((reg) & ~(mask)) | (val)
#define REG_RW_SET1(reg, NAME, val) (reg) = ((reg) & ~NAME##_BITS) | REG_VAL(NAME, val)

#define PIN_SET_TYPE(pin, TYPE) io_bank0_hw->io[pin].ctrl = REG_VAL(IO_BANK0_GPIO0_CTRL_FUNCSEL, GPIO_FUNC_##TYPE)
