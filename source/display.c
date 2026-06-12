/*
 * display.c - game display adapter for the FRDM-KL46Z LCD library.
 */

#include "display.h"
#include "board.h"
#include "lcd.h"

static void show_letter_b(uint8_t digit) {
    turnOnSegment(digit, 'C');
    turnOnSegment(digit, 'D');
    turnOnSegment(digit, 'E');
    turnOnSegment(digit, 'F');
    turnOnSegment(digit, 'G');
}

static void show_letter_e(uint8_t digit) {
    turnOnSegment(digit, 'A');
    turnOnSegment(digit, 'D');
    turnOnSegment(digit, 'E');
    turnOnSegment(digit, 'F');
    turnOnSegment(digit, 'G');
}

static void show_letter_a(uint8_t digit) {
    turnOnSegment(digit, 'A');
    turnOnSegment(digit, 'B');
    turnOnSegment(digit, 'C');
    turnOnSegment(digit, 'E');
    turnOnSegment(digit, 'F');
    turnOnSegment(digit, 'G');
}

static void show_letter_t(uint8_t digit) {
    turnOnSegment(digit, 'D');
    turnOnSegment(digit, 'E');
    turnOnSegment(digit, 'F');
    turnOnSegment(digit, 'G');
}

void display_init(void) {
    LED_RED_INIT(LOGIC_LED_OFF);
    LED_GREEN_INIT(LOGIC_LED_OFF);

    init_lcd();
    clearDisplay();
}

void lcd_clear(void) {
    clearDisplay();
}

void lcd_show_number(int value) {
    if (value < 0) {
        value = 0;
    }
    if (value > 9999) {
        value = 9999;
    }

    clearDisplay();

    if (value >= 1000) {
        displayDigit(1, (value / 1000) % 10);
    }
    if (value >= 100) {
        displayDigit(2, (value / 100) % 10);
    }
    if (value >= 10) {
        displayDigit(3, (value / 10) % 10);
    }
    displayDigit(4, value % 10);
}

void lcd_show_beat(void) {
    clearDisplay();
    show_letter_b(1U);
    show_letter_e(2U);
    show_letter_a(3U);
    show_letter_t(4U);
}

void led_update(uint8_t lane0_near, uint8_t lane1_near) {
    if (lane0_near) {
        LED_GREEN_ON();
    } else {
        LED_GREEN_OFF();
    }

    if (lane1_near) {
        LED_RED_ON();
    } else {
        LED_RED_OFF();
    }
}
