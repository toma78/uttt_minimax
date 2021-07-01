#ifndef MINIMAX_H
#define MINIMAX_H
#include "position.h"

void reset_history();

void age_history();

int minmax(Position *pos, int d, int alpha, int beta);

#endif
