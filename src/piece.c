//
// Created by Raulin Nicolas on 31.12.23.
//

#include "piece.h"


void piece_rotate(struct piece_state *piece) {
	piece->rotation = (piece->rotation + 1) % ROT_TOTAL;
}
