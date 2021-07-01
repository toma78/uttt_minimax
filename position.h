#ifndef POSITION_H
#define POSITION_H
#include "constants.h"

// Numeric representation of board
typedef struct {
	int scnt;
	unsigned int stack[81];
	uint64_t hcode;

	unsigned int nmove, status;
	unsigned int turn, validmb;
	unsigned int p1vics, p2vics, draws;
	unsigned int p1boards[9], p2boards[9];
} Position;

void init_pos(Position *pos, const char *bstr, const char *mbstr, const unsigned int turn, const unsigned int nmove);

void make_move(Position *pos, unsigned int m);

void undo_move(Position *pos, unsigned int m);

int legal_moves(Position *pos, unsigned int *moves);

int victory_moves(Position *pos, unsigned int *moves);

int flagged_moves(Position *pos, unsigned int *moves, unsigned int *flags);

void print_pos(Position *pos);

int mcode2imove(unsigned int m);

// guess opponent's move between positions
unsigned int guess_move(Position *last, Position *current);

int check_pos(Position *pos);

#endif
