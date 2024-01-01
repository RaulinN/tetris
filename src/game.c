//
// Created by Raulin Nicolas on 31.12.23.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

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

enum text_alignment { TEXT_ALIGN_LEFT, TEXT_ALIGN_CENTER, TEXT_ALIGN_RIGHT };

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

static inline int32_t random_int(int32_t min, int32_t max) {
	int32_t range = max - min;
	return min + rand() % range;
}

float get_time_to_next_drop(int32_t level) {
	if (level > LEVEL_MAX_SPEED) {
		level = LEVEL_MAX_SPEED;
	}

	return FRAMES_PER_DROP[level] * TARGET_SECONDS_PER_FRAME;
}

void spawn_piece(struct game_state *game) {
	const uint8_t old_index = game->piece.tetromino_index;

	game->piece = (struct piece_state){};

	// generate a number between 0 and TETROMINO_COUNT + 1 (i.e. 0, 1, 2, ..., 6, 7 even though
	// there is 7 possible tetrominos. The old index is x. Then,
	//  - if the result is not x and is not 7 => accept the result
	//  - if the results is x or 7 => roll a new number between 0 and TETROMINO_COUNT (i.e.
	//    0, 1, ..., 5, 6) and accept the result no matter what
	const uint8_t generated_index = (uint8_t)random_int(0, TETROMINO_COUNT + 1);
	const bool reroll = generated_index == old_index || generated_index == TETROMINO_COUNT;
	if (reroll) {
		game->piece.tetromino_index = (uint8_t)random_int(0, TETROMINO_COUNT);
	} else {
		game->piece.tetromino_index = generated_index;
	}

	LOG_INFO(
			"spawning new piece: old tetromino_index=%d, new tetromino_index=%d (rerolled=%d)",
			old_index, game->piece.tetromino_index, reroll
	);
	game->piece.offset_col = BOARD_WIDTH / 2;
	game->next_drop_time = game->time + get_time_to_next_drop(game->level);
}

bool soft_drop(struct game_state *game) {
	game->piece.offset_row += 1;
	/* if not valid => assume we collided and thus move the piece back up */
	if (!check_piece_valid(&game->piece, &game->board, BOARD_WIDTH, BOARD_HEIGHT)) {
		game->piece.offset_row -= 1;
		merge_piece(game);
		dump_board_state(&game->board, BOARD_WIDTH, BOARD_HEIGHT); // printed only in debug mode
		spawn_piece(game);

		if (!check_row_empty(&game->board, BOARD_WIDTH, 1)) {
			game->phase = GAME_PHASE_GAME_OVER;
		}
		return false;
	}

	game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	return true;
}

void hard_drop(struct game_state *game) {
	while (soft_drop(game));
}

static inline int32_t compute_points(int32_t level, int32_t line_count) {
	switch (line_count) {
		case 1:
			return 40 * (level + 1);
		case 2:
			return 100 * (level + 1);
		case 3:
			return 300 * (level + 1);
		case 4:
			return 1200 * (level + 1);
		default:
			return 0;
	}
}

static inline int32_t min(int32_t x, int32_t y) {
	return x < y ? x : y;
}

static inline int32_t max(int32_t x, int32_t y) {
	return x > y ? x : y;
}

static inline int32_t get_lines_for_next_level(int32_t start_level, int32_t level) {
	int32_t first_level_up_limit = min((start_level * 10 + 10), max(100, (start_level * 10 - 50)));
	if (level == start_level) {
		return first_level_up_limit;
	}

	int32_t diff = level - start_level;
	return first_level_up_limit + diff * 10;
}

void update_game_start(struct game_state *game, const struct input_state *input) {
	if (input->d_up) {
		game->start_level += 1;
	}
	if (input->d_down && game->start_level > 0) {
		game->start_level -= 1;
	}
	if (input->d_a) {
		memset(&game->board, 0, BOARD_WIDTH * BOARD_HEIGHT);
		game->level = game->start_level;
		game->line_count = 0;
		game->points = 0;
		spawn_piece(game);
		game->phase = GAME_PHASE_PLAY;
	}
}

void update_game_game_over(struct game_state *game, const struct input_state *input) {
	if (input->d_a) {
		game->phase = GAME_PHASE_START;
	}
}

void update_game_line(struct game_state *game) {
	if (game->time >= game->highlight_end_time) {
		clear_lines(&game->board, BOARD_WIDTH, BOARD_HEIGHT, game->lines);
		game->line_count += game->pending_line_count;
		game->points += compute_points(game->level, game->pending_line_count);

		int32_t lines_for_next_level = get_lines_for_next_level(game->start_level, game->level);
		if (game->line_count >= lines_for_next_level) {
			game->level += 1;
		}

		game->phase = GAME_PHASE_PLAY;
	}
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

	game->pending_line_count = find_lines(&game->board, BOARD_WIDTH, BOARD_HEIGHT, game->lines);
	if (game->pending_line_count > 0) {
		game->phase = GAME_PHASE_LINE;
		game->highlight_end_time = game->time + 0.5f; // current time +500ms
	}
}

void update_game(struct game_state *game, const struct input_state *input) {
	switch (game->phase) {
		case GAME_PHASE_START:
			update_game_start(game, input);
			break;
		case GAME_PHASE_PLAY:
			update_game_play(game, input);
			break;
		case GAME_PHASE_LINE:
			update_game_line(game);
			break;
		case GAME_PHASE_GAME_OVER:
			update_game_game_over(game, input);
			break;
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

void draw_string(
		SDL_Renderer *renderer,
		TTF_Font *font,
		const char *text,
		int32_t x, int32_t y,
		enum text_alignment alignment,
		struct color text_color
) {
	SDL_Color sdl_color = { text_color.r, text_color.b, text_color.b, text_color.a };
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, sdl_color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect;
	rect.y = y;
	rect.w = surface->w;
	rect.h = surface->h;
	switch (alignment) {
		case TEXT_ALIGN_LEFT:
			rect.x = x;
			break;
		case TEXT_ALIGN_CENTER:
			rect.x = x - surface->w / 2;
			break;
		case TEXT_ALIGN_RIGHT:
			rect.x = x - surface->w;
			break;
	}

	SDL_RenderCopy(renderer, texture, 0, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
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

void render_game(const struct game_state *game, SDL_Renderer *renderer, TTF_Font *font) {
	const int32_t padding_y = 60;

	draw_board(renderer, &game->board, BOARD_WIDTH, BOARD_HEIGHT, 0, padding_y);

	if (game->phase == GAME_PHASE_PLAY) {
		draw_piece(renderer, &game->piece, 0, padding_y);
	}

	struct color colour = (struct color) { .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF };

	if (game->phase == GAME_PHASE_LINE) {
		// line deletion highlighting
		for (int32_t row = 0; row < BOARD_HEIGHT; row += 1) {
			if (game->lines[row]) {
				int32_t x = 0;
				int32_t y = row * GRID_SIZE + padding_y;
				fill_rect(renderer, x, y, BOARD_WIDTH * GRID_SIZE, GRID_SIZE, colour);
			}
		}
	} else if (game->phase == GAME_PHASE_GAME_OVER) {
		int32_t x = BOARD_WIDTH * GRID_SIZE / 2;
		int32_t y = BOARD_HEIGHT * GRID_SIZE / 2;
		draw_string(renderer, font, "GAME OVER", x, y, TEXT_ALIGN_CENTER, colour);
	}

	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "LEVEL: %d", game->level);
	draw_string(renderer, font, buffer, 5, 5, TEXT_ALIGN_LEFT, colour);
}

void start_game(SDL_Renderer *renderer, TTF_Font *font) {
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

		if (key_states[SDL_SCANCODE_ESCAPE]) {
			quit = true;
		}

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
		render_game(&game, renderer, font);

		SDL_RenderPresent(renderer);
	}
}
