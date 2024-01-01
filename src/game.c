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
#include "colors.h"

#define GRID_SIZE 30

#define LEVEL_MAX_SPEED 29


const float TARGET_SECONDS_PER_FRAME = 1.f / 60.f;

const uint8_t FRAMES_PER_DROP[] = {
		48,
		43,
		38,
		33,
		28,
		23,
		18,
		13,
		8,
		6,
		5,
		5,
		5,
		4,
		4,
		4,
		3,
		3,
		3,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		1,
};

struct input_state {
	uint8_t left;
	uint8_t right;
	uint8_t up;
	uint8_t down;
	uint8_t a;
	/* value < 0 if key bound has changed state to being pressed (< 0 if released) */
	int8_t d_left;
	int8_t d_right;
	int8_t d_up;
	int8_t d_down;
	int8_t d_a;
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
			// TODO Check inline
			const uint8_t value = tetromino_get(tetromino, row, col, piece->rotation);
			if (value > 0) {
				int32_t board_row = piece->offset_row + row;
				int32_t board_col = piece->offset_col + col;

				if (board_row < 0) return false;
				if (board_row >= height) return false;
				if (board_col < 0) return false;
				if (board_col >= width) return false;
				if (board_get(board, width, board_row, board_col)) return false;
			}
		}
	}

	return true;
}

void merge_piece(struct game_state *game) {
	const struct tetromino *tetro = TETROMINOS + game->piece.tetromino_index;
	for (int32_t row = 0; row < tetro->size; row += 1) {
		for (int32_t col = 0; col < tetro->size; col += 1) {
			const uint8_t value = tetromino_get(tetro, row, col, game->piece.rotation);
			if (value) {
				board_set(
						&game->board,
						BOARD_WIDTH,
						row + game->piece.offset_row, col + game->piece.offset_col,
						value
				);
			}
		}
	}

}

void spawn_piece(struct game_state *game) {
	game->piece = (struct piece_state){};
	/*game->piece = (struct piece_state) {
		.tetromino_index = 4, // TODO random
		.offset_row = 0,
		.offset_col = BOARD_WIDTH / 2,
		.rotation = ROT_0,
	};*/
}

float get_time_to_next_drop(int32_t level) {
	if (level > LEVEL_MAX_SPEED) {
		level = LEVEL_MAX_SPEED;
	}

	return FRAMES_PER_DROP[level] * TARGET_SECONDS_PER_FRAME;
}

bool soft_drop(struct game_state *game) {
	game->piece.offset_row += 1;
	/* if not valid => assume we collided and thus move the piece back up */
	if (!check_piece_valid(&game->piece, &game->board, BOARD_WIDTH, BOARD_HEIGHT)) {
		game->piece.offset_row -= 1;
		merge_piece(game);
		spawn_piece(game);
		return false;
	}

	game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	return true;
}

void hard_drop(struct game_state *game) {
	while (soft_drop(game));
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

	// TODO might be sus (&game->board)
	if (check_piece_valid(&piece, &game->board, BOARD_WIDTH, BOARD_HEIGHT)) {
		game->piece = piece; /* update the state of the piece */
	}

	if (input->d_down > 0) {
		soft_drop(game);
	}

	if (input->d_a > 0) {
		hard_drop(game);
	}

	while (game->time >= game->next_drop_time) {
		soft_drop(game);
	}
}

void update_game(struct game_state *game, const struct input_state *input) {
	switch (game->phase) {
		case GAME_PHASE_PLAY:
			return update_game_play(game, input);
	}
}

void fill_rect(
		SDL_Renderer *renderer,
		int32_t x, int32_t y,
		int32_t width, int32_t height,
		struct color color_value
		) {
	SDL_Rect rect = { .x = x, .y = y, .w = width, .h = height };
	SDL_SetRenderDrawColor(renderer, color_value.r, color_value.g, color_value.b, color_value.a);
	SDL_RenderFillRect(renderer, &rect);
}

void draw_cell(
		SDL_Renderer *renderer,
		int32_t row, int32_t col,
		uint8_t value,
		int32_t offset_x, int32_t offset_y
		) {
	struct color base_color =  BASE_COLORS[value];
	struct color light_color = LIGHT_COLORS[value];
	struct color dark_color =  DARK_COLORS[value];

	const int32_t edge = GRID_SIZE / 8;
	const int32_t x = col * GRID_SIZE + offset_x;
	const int32_t y = row * GRID_SIZE + offset_y;

	fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color);
	fill_rect(renderer, x + edge, y, GRID_SIZE - edge, GRID_SIZE - edge, light_color);
	fill_rect(renderer, x + edge, y + edge, GRID_SIZE - 2 * edge, GRID_SIZE - 2 * edge, base_color);
}

void draw_piece(SDL_Renderer *renderer, const struct piece_state *piece, int32_t offset_x, int32_t offset_y) {
	const struct tetromino *tetro = TETROMINOS + piece->tetromino_index;
	for (int32_t row = 0; row < tetro->size; row += 1) {
		for (int32_t col = 0; col < tetro->size; col += 1) {
			uint8_t value = tetromino_get(tetro, row, col, piece->rotation);
			if (value) {
				draw_cell(renderer, piece->offset_row + row,piece->offset_col + col, value, offset_x, offset_y);
			}
		}
	}
}

void draw_board(
		SDL_Renderer *renderer,
		const struct board_state *board,
		int32_t width, int32_t height,
		int32_t offset_x, int32_t offset_y
		) {
	for (int32_t row = 0; row < height; row += 1) {
		for (int32_t col = 0; col < width; col += 1) {
			const uint8_t value = board_get(board, width, row, col);
			draw_cell(renderer, row, col, value, offset_x, offset_y);
			/*if (value) {
				draw_cell(renderer, row, col, value, offset_x, offset_y);
			}*/
		}
	}
}

void render_game(const struct game_state *game, SDL_Renderer *renderer) {
	draw_board(renderer, &game->board, BOARD_WIDTH, BOARD_HEIGHT, 0, 0);
	draw_piece(renderer, &game->piece, 0, 0);
}

void start_game(SDL_Renderer *renderer) {
	/* states are pretty small => keep it simple and use the stack */
	struct game_state game = {};
	struct input_state input = {};

	spawn_piece(&game);

	int quit = 0;
	while (!quit) {
		game.time = SDL_GetTicks() / 1000.0f;

		SDL_Event e;
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			}
		}

		int32_t key_count;
		const uint8_t *key_states = SDL_GetKeyboardState(&key_count);

		struct input_state prev_input = input;

		input.left = key_states[SDL_SCANCODE_LEFT];
		input.right = key_states[SDL_SCANCODE_RIGHT];
		input.up = key_states[SDL_SCANCODE_UP];
		input.down = key_states[SDL_SCANCODE_DOWN];
		input.a = key_states[SDL_SCANCODE_SPACE];

		input.d_left = input.left - prev_input.left;
		input.d_right = input.right - prev_input.right;
		input.d_up = input.up - prev_input.up;
		input.d_down = input.down - prev_input.down;
		input.d_a = input.a - prev_input.a;


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		update_game(&game, &input);
		render_game(&game, renderer);

		SDL_RenderPresent(renderer);
	}
}
