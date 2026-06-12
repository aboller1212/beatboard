#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/* Configure SysTick to fire every 10 ms and start counting. */
void timer_init(void);

/* Returns elapsed time in milliseconds since timer_init(). */
uint32_t get_time_ms(void);

#endif /* TIMER_H_ */
