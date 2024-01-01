//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <stdint.h>


enum tetromino_rotation {
	ROT_0,
	ROT_90,
	ROT_180,
	ROT_270,
	ROT_TOTAL,
};

struct tetromino {
	const uint8_t *data; 		/* represents the shape of the tetromino */
	const int32_t size;			/* dimension N of the matrix NxN used to represent the tetromino */
};

extern const struct tetromino TETROMINOS[];

uint8_t tetromino_get(const struct tetromino *tetromino, int32_t row, int32_t col, enum tetromino_rotation rotation);
