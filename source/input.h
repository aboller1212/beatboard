#ifndef INPUT_H_
#define INPUT_H_

#include <stdint.h>

/* Configure GPIO for SW1 (lane 0) and SW3 (lane 1). */
void input_init(void);

/* Poll both buttons and run debounce state machine. Call once per 10 ms tick. */
void update_inputs(void);

/* Returns 1 and writes *press_time_out if a confirmed press is pending for
 * the given lane (0 = SW1, 1 = SW3). Clears the flag — each press returned once. */
uint8_t button_get_press(uint8_t lane, uint32_t *press_time_out);

#endif /* INPUT_H_ */
