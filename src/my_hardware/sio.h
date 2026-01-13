#pragma once

#include <hardware/regs/io_bank0.h>
#include <hardware/structs/io_bank0.h>
#include <hardware/structs/sio.h>

#include "utils.h"

#define SIO_SET(pin) sio_hw->gpio_set = BIT(pin)
#define SIO_CLEAR(pin) sio_hw->gpio_clr = BIT(pin)
#define SIO_SET_OUT(pin) sio_hw->gpio_oe_set = BIT(pin); SIO_CLEAR(pin)
#define SIO_IN_GET(pin) ((sio_hw->gpio_in >> pin) & 1)
#define SIO_OUT_GET(pin) ((sio_hw->gpio_out >> pin) & 1)
