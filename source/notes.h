#ifndef NOTES_H_
#define NOTES_H_

#include <stdint.h>

typedef struct {
    uint32_t time_ms; /* scheduled beat time, relative to game start */
    uint8_t  lane;    /* 0 = left (SW1), 1 = right (SW3) */
    uint8_t  hit;     /* 0 = pending, 1 = hit, 2 = missed */
} note_t;

extern note_t       song[];
extern const int    SONG_LENGTH;

/* Reset all notes to hit=0 before a new game. */
void notes_reset(void);

/* Mark pending notes whose late window (note_time + 250 ms) has passed as missed. */
void notes_check_miss(uint32_t current_time_ms);

/* Return index of the nearest pending note for the given lane within ±250 ms,
 * or -1 if none is close enough. */
int notes_find_active(uint32_t current_time_ms, uint8_t lane);

#endif /* NOTES_H_ */
