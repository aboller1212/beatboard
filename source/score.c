#include "score.h"

void score_reset(game_score_t *gs) {
    gs->score = 0;
    gs->combo = 0;
}

uint8_t score_process_hit(note_t *note, uint32_t press_time, game_score_t *gs) {
    uint32_t diff = (press_time >= note->time_ms)
                  ? (press_time     - note->time_ms)
                  : (note->time_ms  - press_time);

    if (diff <= PERFECT_WINDOW_MS) {
        note->hit   = 1U;
        gs->score  += PERFECT_POINTS;
        gs->combo++;
        return 1U;
    }
    if (diff <= GOOD_WINDOW_MS) {
        note->hit   = 1U;
        gs->score  += GOOD_POINTS;
        gs->combo++;
        return 1U;
    }
    /* Press landed outside both windows — count as a miss. */
    score_process_miss(gs);
    return 0U;
}

void score_process_miss(game_score_t *gs) {
    gs->combo = 0;
}
