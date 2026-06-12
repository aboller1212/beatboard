#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include <stdint.h>
#include "notes.h"
#include "score.h"

typedef enum {
    STATE_MENU,
    STATE_COUNTDOWN,
    STATE_PLAYING,
    STATE_RESULTS,
} game_state_t;

typedef struct {
    uint32_t      current_time; /* snapshot of get_time_ms() each update */
    uint32_t      start_time;   /* system_ms when STATE_PLAYING began     */
    int           note_index;   /* next unprocessed note for miss scanning */
    game_state_t  state;
    game_score_t  gs;           /* score + combo */
} game_t;

/* Initialise the FSM. Call once after all hardware inits. */
void game_init(void);

/* Advance the FSM by one tick. Call from the main loop. */
void update_game_state(void);

/* Expose read-only view for main.c / display calls. */
const game_t *game_get(void);

#endif /* GAME_STATE_H_ */
