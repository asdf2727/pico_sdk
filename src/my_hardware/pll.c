#include "pll.h"

#include <hardware/structs/pll.h>

#include "utils.h"

#define PLL_FREQ(PLL) (float)XOSC_HZ / \
        (REG_GET(PLL->prim, PLL_PRIM_POSTDIV1) * \
        REG_GET(PLL->prim, PLL_PRIM_POSTDIV2) * \
        REG_GET(PLL->cs, PLL_CS_REFDIV)) * \
        REG_GET(PLL->fbdiv_int, PLL_FBDIV_INT)

float sys_freq() {
    return PLL_FREQ(pll_sys_hw);
}
float usb_freq() {
    return PLL_FREQ(pll_sys_hw);
}