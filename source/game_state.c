#include "game_state.h"
#include "timer.h"
#include "input.h"
#include "notes.h"
#include "score.h"
#include "display.h"
#include "board.h"   /* LED macros for diagnostic LED flashes */

/* Duration of the 3-second countdown before notes begin. */
#define COUNTDOWN_MS       3000U

/* A note's LED proximity window: light up the lane LED this many ms around beat. */
#define LED_PROXIMITY_MS   200U

/* The song ends this many ms after the last note's beat time. */
#define SONG_END_BUFFER_MS 1000U

/* How many ms the RESULTS screen stays visible before returning to MENU. */
#define RESULTS_DISPLAY_MS 4000U

static game_t g;

/* Rate-limit LCD redraws to LCD_UPDATE_INTERVAL_MS. */
static uint32_t last_lcd_update_ms = 0U;

/* ── Internal helpers ───────────────────────────────────────────────────────── */

/* Convert an absolute note index to a song-relative time by adding start_time. */
static uint32_t note_abs_time(int idx) {
    return g.start_time + song[idx].time_ms;
}

/* Return 1 if any lane-0 note is within LED_PROXIMITY_MS of current_time. */
static uint8_t lane_near(uint8_t lane) {
    for (int i = 0; i < SONG_LENGTH; i++) {
        if (song[i].hit != 0U)         continue;
        if (song[i].lane != lane)       continue;
        uint32_t abs_t = note_abs_time(i);
        uint32_t diff  = (g.current_time >= abs_t)
                       ? (g.current_time - abs_t)
                       : (abs_t - g.current_time);
        if (diff <= LED_PROXIMITY_MS) return 1U;
    }
    return 0U;
}

/* Drive the LCD, rate-limited. */
static void refresh_display(void) {
    if ((g.current_time - last_lcd_update_ms) < LCD_UPDATE_INTERVAL_MS) return;
    last_lcd_update_ms = g.current_time;

    switch (g.state) {
        case STATE_MENU:
        case STATE_COUNTDOWN:
            lcd_show_beat();
            break;
        case STATE_PLAYING:
            lcd_show_number(g.gs.score);
            break;
        case STATE_RESULTS:
            lcd_show_number(g.gs.score);
            break;
        default:
            break;
    }
}

/* ── State handlers ─────────────────────────────────────────────────────────── */

static void handle_menu(void) {
    /* Menu state: both LEDs OFF. Either button starts a new game. */
    LED_RED_OFF();
    LED_GREEN_OFF();
    uint32_t t;
    if (button_get_press(0U, &t) || button_get_press(1U, &t)) {
        g.state = STATE_COUNTDOWN;
        g.start_time = g.current_time;
        last_lcd_update_ms = 0U;
    }
}

static void handle_countdown(void) {
    /* Countdown state: BOTH LEDs ON solidly for 3 seconds — clear visual cue
     * that the game is starting and the menu→countdown transition fired.    */
    LED_RED_ON();
    LED_GREEN_ON();

    if ((g.current_time - g.start_time) >= COUNTDOWN_MS) {
        notes_reset();
        score_reset(&g.gs);
        g.note_index = 0;
        g.start_time = g.current_time;
        g.state      = STATE_PLAYING;
        last_lcd_update_ms = 0U;
    }
}

static void handle_playing(void) {
    /* Relative time within the song (ms since game started). */
    uint32_t song_time = g.current_time - g.start_time;

    /* Mark notes that slipped past their late window as missed. */
    notes_check_miss(song_time);

    /* Process button presses for both lanes. */
    for (uint8_t lane = 0U; lane < 2U; lane++) {
        uint32_t press_abs;
        if (!button_get_press(lane, &press_abs)) continue;

        /* press_abs is absolute ms; convert to song-relative. */
        uint32_t press_song = press_abs - g.start_time;

        int idx = notes_find_active(press_song, lane);
        if (idx >= 0) {
            score_process_hit(&song[idx], press_song, &g.gs);
        } else {
            /* Press with no nearby note — penalty miss. */
            score_process_miss(&g.gs);
        }
    }

    /* LED proximity indicators. */
    led_update(lane_near(0U), lane_near(1U));

    /* Detect song end: all notes processed and last note + buffer has elapsed. */
    uint32_t last_note_end = song[SONG_LENGTH - 1].time_ms + SONG_END_BUFFER_MS;
    if (song_time >= last_note_end) {
        led_update(0U, 0U); /* LEDs off */
        g.start_time = g.current_time; /* reuse start_time as results timer */
        g.state      = STATE_RESULTS;
        last_lcd_update_ms = 0U;
    }
}

static void handle_results(void) {
    /* Results state: red LED ON to indicate "game over, showing score". */
    LED_RED_ON();
    LED_GREEN_OFF();
    if ((g.current_time - g.start_time) >= RESULTS_DISPLAY_MS) {
        g.state = STATE_MENU;
        lcd_show_beat();
        last_lcd_update_ms = 0U;
    }
}

/* ── Public API ─────────────────────────────────────────────────────────────── */

void game_init(void) {
    g.state        = STATE_MENU;
    g.start_time   = 0U;
    g.current_time = 0U;
    g.note_index   = 0;
    score_reset(&g.gs);
    notes_reset();
    lcd_show_beat();
}

void update_game_state(void) {
    g.current_time = get_time_ms();

    switch (g.state) {
        case STATE_MENU:       handle_menu();       break;
        case STATE_COUNTDOWN:  handle_countdown();  break;
        case STATE_PLAYING:    handle_playing();     break;
        case STATE_RESULTS:    handle_results();     break;
        default:               g.state = STATE_MENU; break;
    }

    refresh_display();
}

const game_t *game_get(void) {
    return &g;
}
