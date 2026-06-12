#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/* Initialise SLCD peripheral and LEDs. Call after BOARD_InitBootPins(). */
void display_init(void);

/* Show a 4-digit decimal value on the LCD (0-9999). */
void lcd_show_number(int value);

/* Show the "BEAt" splash string on the LCD (used in MENU state). */
void lcd_show_beat(void);

/* Clear all LCD segments. */
void lcd_clear(void);

/* Drive LEDs based on lane proximity.
 * lane0_near / lane1_near: 1 = a lane-0/lane-1 note is within 200 ms. */
void led_update(uint8_t lane0_near, uint8_t lane1_near);

#define LCD_UPDATE_INTERVAL_MS 50U

#endif /* DISPLAY_H_ */
