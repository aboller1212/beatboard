#include "notes.h"

#define MISS_WINDOW_MS 250U

/* Song beatmap — times are ms after game start, not absolute system time.
 * game_state.c offsets these by g.start_time when comparing. */
note_t song[] = {
    {1000U, 0U, 0U},
    {2000U, 1U, 0U},
    {3000U, 0U, 0U},
    {4000U, 1U, 0U},
    {5000U, 0U, 0U},
    {6000U, 1U, 0U},
    {7000U, 0U, 0U},
    {8000U, 1U, 0U},
};
const int SONG_LENGTH = 8;

void notes_reset(void) {
    for (int i = 0; i < SONG_LENGTH; i++) {
        song[i].hit = 0U;
    }
}

void notes_check_miss(uint32_t current_time_ms) {
    for (int i = 0; i < SONG_LENGTH; i++) {
        if (song[i].hit != 0U) continue;
        if (current_time_ms > song[i].time_ms + MISS_WINDOW_MS) {
            song[i].hit = 2U; /* missed */
        }
    }
}

int notes_find_active(uint32_t current_time_ms, uint8_t lane) {
    int      best      = -1;
    uint32_t best_diff = MISS_WINDOW_MS + 1U;

    for (int i = 0; i < SONG_LENGTH; i++) {
        if (song[i].hit  != 0U)    continue;
        if (song[i].lane != lane)  continue;

        uint32_t diff = (current_time_ms >= song[i].time_ms)
                      ? (current_time_ms - song[i].time_ms)
                      : (song[i].time_ms  - current_time_ms);

        if (diff <= MISS_WINDOW_MS && diff < best_diff) {
            best      = i;
            best_diff = diff;
        }
    }
    return best;
}
