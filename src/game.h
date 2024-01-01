//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <SDL.h>

#include "board.h"
#include "piece.h"


enum game_phase {
	GAME_PHASE_PLAY,
};

struct game_state {
	struct board_state board;
	struct piece_state piece;
	enum game_phase phase;

	int32_t level;

	float next_drop_time;
	float time;
};

void start_game(SDL_Renderer *renderer);
