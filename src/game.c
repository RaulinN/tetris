//
// Created by Raulin Nicolas on 31.12.23.
//

#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

#include "game.h"
#include "piece.h"
#include "tetromino.h"
#include "log.h"


struct input_state {
	/* value < 0 if key bound has changed state to being pressed (< 0 if released) */
	int8_t d_left;
	int8_t d_right;
	int8_t d_up;
};

bool check_piece_valid(const struct piece_state *piece, const struct board_state *board, int32_t width, int32_t height) {
	const struct tetromino *tetromino = TETROMINOS + piece->tetromino_index;
	if (tetromino == NULL) {
		LOG_ERROR("failed to fetch tetromino_index=%d from TETROMINOS array", piece->tetromino_index);
		exit(EXIT_FAILURE);
	}

	/* check whether the piece is valid (i.e. not colliding with other pieces or walls) */
	/* row and col are indexes of the tetromino matrix */
	for (int32_t row = 0; row < tetromino->size; row += 1) {
		for (int32_t col = 0; col < tetromino->size; col += 1) {
			const uint8_t value = tetromino_get(tetromino, row, col, piece->rotation);
			if (value > 0) {
				int32_t board_row = piece->offset_row + row;
				int32_t board_col = piece->offset_col + col;

				if (board_row < 0) return false;
				if (board_row >= height) return false;
				if (board_col < 0) return false;
				if (board_col >= width) return false;
				//if (board_get(board, width, board_row, board_col)) return false;
			}
		}
	}

	return true;
}

void update_game_play(struct game_state *game, const struct input_state *input) {
	struct piece_state piece = game->piece;

	if (input->d_left > 0) {
		piece.offset_col -= 1;
	}
	if (input->d_right > 0) {
		piece.offset_col += 1;
	}
	if (input->d_up > 0) {
		piece_rotate(&piece);
	}
	/*if (input->d_down > 0) {

	}*/
	// TODO might be sus (&game->board)
	if (check_piece_valid(&piece, &game->board, BOARD_WIDTH, BOARD_HEIGHT)) {
		game->piece = piece; /* update the state of the piece */
	}
}

void update_game(struct game_state *game, const struct input_state *input) {
	switch (game->phase) {
		case GAME_PHASE_PLAY:
			return update_game_play(game, input);
	}
}

void render_game(const struct game_state *game, SDL_Renderer *renderer) {

}

void start_game(SDL_Renderer *renderer) {
	/* states are pretty small => keep it simple and use the stack */
	struct game_state game = {};
	struct input_state input = {};

	int quit = 0;
	while (!quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		update_game(&game, &input);
		render_game(&game, renderer);

		SDL_RenderPresent(renderer);
	}
}
