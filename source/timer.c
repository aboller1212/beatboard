#include "timer.h"
#include "fsl_device_registers.h" /* provides SysTick, SystemCoreClock */

/* Incremented by 10 in each SysTick ISR. volatile: written in ISR, read in main. */
static volatile uint32_t system_time_ms = 0U;

void timer_init(void) {
    /* SystemCoreClock is set by BOARD_InitBootClocks() before this is called. */
    SysTick->LOAD = (SystemCoreClock / 100U) - 1U; /* 10 ms period */
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk   /* use processor clock */
                  | SysTick_CTRL_TICKINT_Msk      /* enable interrupt */
                  | SysTick_CTRL_ENABLE_Msk;      /* start counter */
}

uint32_t get_time_ms(void) {
    return system_time_ms;
}

/* Overrides the weak default in startup_MKL46Z4.S. */
void SysTick_Handler(void) {
    system_time_ms += 10U;
}
