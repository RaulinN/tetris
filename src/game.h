//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <SDL.h>

#include "board.h"
#include "piece.h"


enum game_phase {
	GAME_PHASE_PLAY,					/* game phase: playing tetris */
	GAME_PHASE_LINE,					/* game phase: currently clearing lines (highlighting) */
	GAME_PHASE_GAME_OVER,				/* game phase: game is over (lost) */
};

struct game_state {
	struct board_state board;			/* current state of the board */
	uint8_t lines[BOARD_HEIGHT]; 		/* array of filled lines 1 = filled, 0 = empty */
	int32_t pending_line_count;			/* number of lines to be cleared */

	struct piece_state piece;			/* current piece falling */
	enum game_phase phase;				/* current state of the game */

	int32_t level;						/* current level of the game */
	int32_t start_level;				/* TODO */
	int32_t line_count;					/* total number of lines cleared during the game */
	int32_t points;						/* total number of points achieved */

	float next_drop_time;				/* time when to next soft_drop */
	float highlight_end_time;			/* time when to stop highlighting lines that were cleared */
	float time;							/* current time */
};

void start_game(SDL_Renderer *renderer);
