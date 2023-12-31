//
// Created by Raulin Nicolas on 31.12.23.
//

#pragma once

#include <stdint.h>

#include "tetromino.h"


struct piece_state {
	uint8_t tetromino_index;					/* tetromino index in the global TETROMINOS table */
	int32_t offset_row;							/* piece row offset on the board when falling */
	int32_t offset_col;							/* piece column offset on the board when falling */
	enum tetromino_rotation rotation;			/* piece rotation on the board when falling */
};

void piece_rotate(struct piece_state *piece);
