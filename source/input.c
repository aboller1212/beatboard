#include "input.h"
#include "timer.h"
#include "board.h"      /* BOARD_SW1_GPIO, BOARD_SW1_GPIO_PIN, etc. */
#include "fsl_gpio.h"

/* Two consecutive stable samples required before state transition is accepted. */
#define DEBOUNCE_THRESHOLD 2U

typedef struct {
    uint8_t  raw;           /* last raw reading (0 = pressed, active-low) */
    uint8_t  stable;        /* debounced state (0 = pressed) */
    uint8_t  count;         /* consecutive samples matching raw */
    uint8_t  press_pending; /* 1 when a falling-edge press is waiting to be consumed */
    uint32_t press_time;    /* get_time_ms() value at moment of confirmed press */
} btn_state_t;

/* [0] = SW1 (lane 0 / left), [1] = SW3 (lane 1 / right) */
static btn_state_t buttons[2];

void input_init(void) {
    /* Pin mux and pull-ups are already configured by BOARD_InitBootPins() →
     * BOARD_InitButtons() in pin_mux.c.  We only need GPIO direction here.
     * Both SW1 (PTC3) and SW3 (PTC12) share GPIOC. */
    gpio_pin_config_t in_cfg = {kGPIO_DigitalInput, 0U};
    GPIO_PinInit(BOARD_SW1_GPIO, BOARD_SW1_GPIO_PIN, &in_cfg);
    GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN, &in_cfg);

    /* Assume released (logic 1) at power-on. */
    buttons[0].stable = 1U;
    buttons[1].stable = 1U;
}

void update_inputs(void) {
    uint8_t raw[2];
    raw[0] = (uint8_t)GPIO_PinRead(BOARD_SW1_GPIO, BOARD_SW1_GPIO_PIN);
    raw[1] = (uint8_t)GPIO_PinRead(BOARD_SW3_GPIO, BOARD_SW3_GPIO_PIN);

    for (uint8_t i = 0U; i < 2U; i++) {
        if (raw[i] == buttons[i].raw) {
            /* Same reading as last call — accumulate stable count. */
            if (buttons[i].count < DEBOUNCE_THRESHOLD) {
                buttons[i].count++;
            }
        } else {
            /* Reading changed — restart counter with new value. */
            buttons[i].raw   = raw[i];
            buttons[i].count = 0U;
        }

        if (buttons[i].count >= DEBOUNCE_THRESHOLD) {
            uint8_t prev = buttons[i].stable;
            buttons[i].stable = buttons[i].raw;

            /* Falling edge (released → pressed) = button press event. */
            if ((prev == 1U) && (buttons[i].stable == 0U)) {
                buttons[i].press_pending = 1U;
                buttons[i].press_time    = get_time_ms();
            }
        }
    }
}

uint8_t button_get_press(uint8_t lane, uint32_t *press_time_out) {
    if (lane > 1U) return 0U;
    if (buttons[lane].press_pending) {
        *press_time_out              = buttons[lane].press_time;
        buttons[lane].press_pending  = 0U;
        return 1U;
    }
    return 0U;
}
