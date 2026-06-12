#ifndef SCORE_H_
#define SCORE_H_

#include <stdint.h>
#include "notes.h"

#define PERFECT_WINDOW_MS  100U
#define GOOD_WINDOW_MS     250U
#define PERFECT_POINTS     100
#define GOOD_POINTS         50

typedef struct {
    int score;
    int combo;
} game_score_t;

void score_reset(game_score_t *gs);

/* Classify the hit against note->time_ms.
 * On perfect/good: sets note->hit = 1, increments score and combo, returns 1.
 * Outside window:  resets combo, returns 0.
 * press_time is absolute ms (already offset from start_time by caller). */
uint8_t score_process_hit(note_t *note, uint32_t press_time, game_score_t *gs);

/* Call when a note is confirmed missed (no button press in window). */
void score_process_miss(game_score_t *gs);

#endif /* SCORE_H_ */
