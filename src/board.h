//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <stdint.h>


#define BOARD_WIDTH 10
#define BOARD_VISIBLE_HEIGHT 20
#define BOARD_HEIGHT (BOARD_VISIBLE_HEIGHT + 2) 	/* including 2 hidden rows to spawn pieces */

struct board_state {
	uint8_t data[BOARD_WIDTH * BOARD_HEIGHT]; 		/* 0 => empty cell, 1+ => cell containing a block */
};

inline uint8_t board_get(const struct board_state *board, int32_t width, int32_t row, int32_t col);
inline void board_set(struct board_state *board, uint32_t width, int32_t row, int32_t col, int8_t value);
