//
// Created by Raulin Nicolas on 31.12.23.
//

#include "board.h"


inline uint8_t board_get(const struct board_state *board, int32_t width, int32_t row, int32_t col) {
	const int32_t index = row * width + col;
	return board->data[index];
}

inline void board_set(struct board_state *board, uint32_t width, int32_t row, int32_t col, int8_t value) {
	const int32_t index = row * width + col;
	board->data[index] = value;
}