//
// Created by Raulin Nicolas on 31.12.23.
//

#include <stdlib.h>

#include "tetromino.h"
#include "log.h"


/* tetromino bar (straight tetromino) */
const uint8_t TETROMINO_1[] = {
	0, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 0,
	0, 0, 0, 0,
};

/* tetromino square (square tetromino) */
const uint8_t TETROMINO_2[] = {
	2, 2,
	2, 2,
};

/* tetromino T (T-tetromino) */
const uint8_t TETROMINO_3[] = {
	0, 0, 0,
	3, 3, 3,
	0, 3, 0,
};

/* tetromino L (L-tetromino) */
const uint8_t TETROMINO_4[] = {
	4, 0, 0,
	4, 0, 0,
	4, 4, 0,
};

/* tetromino Z (skew tetromino) */
const uint8_t TETROMINO_5[] = {
	0, 5, 5,
	5, 5, 0,
	0, 0, 0,
};

const struct tetromino TETROMINOS[] = {
	(struct tetromino) { .data = TETROMINO_1, .size = 4 }, /* index 0 : straight tetromino */
	(struct tetromino) { .data = TETROMINO_2, .size = 2 }, /* index 1 : square tetromino */
	(struct tetromino) { .data = TETROMINO_3, .size = 3 }, /* index 2 : T-tetromino */
	(struct tetromino) { .data = TETROMINO_4, .size = 3 }, /* index 3 : L-tetromino */
	(struct tetromino) { .data = TETROMINO_5, .size = 3 }, /* index 4 : skew tetromino */
};

uint8_t tetromino_get(const struct tetromino *tetromino, int32_t row, int32_t col, enum tetromino_rotation rotation) {
	const int32_t size = tetromino->size;
	switch (rotation) {
		case ROT_0: /* no rotation */
			return tetromino->data[row * size + col];
		case ROT_90: /* 90 deg rotation: r = size - c' - 1, c = r' */
			return tetromino->data[(size - col - 1) * size + (row)];
		case ROT_180: /* 180 deg rotation r = size - r' - 1, c = size - c' - 1 */
			return tetromino->data[(size - row - 1) * size + (size - col - 1)];
		case ROT_270: /* 270 deg rotation r = c', c = size - r' - 1*/
			return tetromino->data[(col) * size + (size - row - 1)];
		default:
			LOG_ERROR("fatal error getting tetromino for rotation=%d (total of %d rotations)", rotation, ROT_TOTAL);
			exit(EXIT_FAILURE);
	}
}
