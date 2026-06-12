/*
 * BeatBoard — real-time rhythm game for FRDM-KL46Z
 *
 * FSM: MENU → COUNTDOWN → PLAYING → RESULTS → MENU
 *
 * Entry point and main loop.  All timing is ISR-driven via SysTick; this loop
 * never calls delay functions.
 */

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"

#include "timer.h"
#include "input.h"
#include "display.h"
#include "game_state.h"
#include "lcd.h"

/* Initialise all subsystems in dependency order. */
static void init_all(void) {
    /* NXP SDK board init: clocks, pin mux, peripherals. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    /* The MCUXpresso config-tool-generated BOARD_InitBootPins() does NOT
     * call the per-functional-group pin inits — it only calls BOARD_InitPins()
     * (empty) and BOARD_InitDEBUG_UART(). We must call the others manually
     * to configure the pin MUX for buttons, LEDs, and the segment LCD. */
    BOARD_InitButtons();      /* PTC3 (SW1), PTC12 (SW3) GPIO + pull-up */
    BOARD_InitLEDs();         /* PTD5 (green), PTE29 (red) GPIO output  */
    BOARD_InitSegment_LCD();  /* 12 LCD_Pxx pins to SLCD analog mode    */

    /* Game subsystems — must come after SDK board init. */
    timer_init();       /* start SysTick at 10 ms intervals       */
    input_init();       /* configure SW1 / SW3 GPIO inputs         */
    display_init();     /* configure SLCD and LEDs                 */
    game_init();        /* reset FSM, show "BEAt" splash           */
}

/* === DIAGNOSTIC MODE ==============================================
 * AUTO-CYCLES through LCD test patterns every 2 seconds, no buttons
 * needed. Watch and write down what you see at each step. The red LED
 * blinks once per second so you can see the loop is alive. */
#define DIAGNOSTIC_MODE 0  /* 0 = real game; 1 = old LCD raw test; 2 = raw buttons; 3 = LCD library test */

extern void lcd_clear(void);
extern void lcd_write_digit_raw(uint8_t pos, uint8_t seg_val);

int main(void) {
    init_all();

#if DIAGNOSTIC_MODE == 2
    /* RAW BUTTON TEST: directly read GPIO pins, no debounce, no game logic.
     * SW1 held → red LED ON. SW3 held → green LED ON. Released → both OFF.   */
    LED_GREEN_OFF();
    LED_RED_OFF();
    while (1) {
        /* Active-low buttons: PinRead returns 0 when pressed. */
        if (GPIO_PinRead(BOARD_SW1_GPIO, BOARD_SW1_GPIO_PIN) == 0U) {
            LED_RED_ON();
        } else {
            LED_RED_OFF();
        }
        if (GPIO_PinRead(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN) == 0U) {
            LED_GREEN_ON();
        } else {
            LED_GREEN_OFF();
        }
    }
#elif DIAGNOSTIC_MODE == 3
    LED_GREEN_OFF();
    LED_RED_OFF();

    clearDisplay();
    displayDigit(1, 3);
    displayDigit(2, 1);
    displayDigit(3, 4);
    displayDigit(4, 0);
    LED_GREEN_ON();

    while (1) {
        LCD_TimeDelay(0x7FFFFU);
        LED_RED_TOGGLE();
    }
#elif DIAGNOSTIC_MODE
    LED_GREEN_OFF();
    LED_RED_OFF();

    uint32_t last_red_toggle = 0;
    uint32_t last_lcd_refresh = 0;
    uint8_t red_on = 0;

    while (1) {
        update_inputs();
        uint32_t now = get_time_ms();

        /* Continuously refresh "8888" every 100 ms. If something is wiping
         * the WF8B values, this will keep them visible.                    */
        if (now - last_lcd_refresh >= 100U) {
            last_lcd_refresh = now;
            lcd_clear();
            lcd_write_digit_raw(0, 0xFFU);
            lcd_write_digit_raw(1, 0xFFU);
            lcd_write_digit_raw(2, 0xFFU);
            lcd_write_digit_raw(3, 0xFFU);
        }

        if (now - last_red_toggle >= 500U) {
            last_red_toggle = now;
            red_on = !red_on;
            if (red_on) { LED_RED_ON(); } else { LED_RED_OFF(); }
        }

        uint32_t t;
        if (button_get_press(0, &t) || button_get_press(1, &t)) {
            LED_GREEN_TOGGLE();
        }
    }
#else
    while (1) {
        update_inputs();
        update_game_state();
    }
#endif
    return 0;
}
