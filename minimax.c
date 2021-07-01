#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "evaluate.h"
#include "hash.h"
#include "triplebot.h"
#include "minimax.h"

#define MHASH_VALUE    1000000
#define MVIC_VALUE     100000
#define MTHREAT_VALUE  100 //60
#define MBLOCK_VALUE   110 //50
#define MOVER_VALUE    -1000 //-180

const int move_value[16] = {   
    0,                                  // 0000 no flags
    MVIC_VALUE,                         // 0001 victory
    MTHREAT_VALUE,                      // 0010 threat
    MVIC_VALUE,                         // 0011 vic+threat
    MBLOCK_VALUE,                       // 0100 tblock
    MVIC_VALUE,                         // 0101 vic+tblock
    MTHREAT_VALUE,                      // 0110 threat+tblock
    MVIC_VALUE,                         // 0111 vic+threat+tblock
    MOVER_VALUE,                        // 1000 over 
    MOVER_VALUE+MVIC_VALUE,             // 1001 over+victory
    MOVER_VALUE+MTHREAT_VALUE,          // 1010 over+threat
    MOVER_VALUE+MVIC_VALUE,             // 1011 over+vic+threat
    MOVER_VALUE+MBLOCK_VALUE,           // 1100 over+tblock
    MOVER_VALUE+MVIC_VALUE,             // 1101 over+vic+tblock
    MOVER_VALUE+MTHREAT_VALUE,          // 1110 over+threat+tblock
    MOVER_VALUE+MVIC_VALUE              // 1111 over+vic+threat+tblock
};

int histmax = 1;
int history[256] = { 0 };

void reset_history() {
	histmax = 1;
	for (int i = 0; i<256; i++)
		history[i] = 0;
}

void age_history() {
	histmax = 1;
	for (int i = 0; i < 256; i++) {
		history[i] >>= 1;
		histmax = (history[i] > histmax) ? history[i] : histmax;
	}
}

void increase_hist(int mc, int d) {
	history[mc] += d*d*d;
	histmax = (history[mc] > histmax) ? history[mc] : histmax;
}

int hist_value(int mc) {
	return (100 * history[mc]) / histmax;
}

int sorted_moves(Position *pos, unsigned int *moves, unsigned int *flags) {
	unsigned int hmove = get_ht_move(pos->hcode, pos->nmove);
	// generate and evaluate moves
	int values[81];
	int nm = flagged_moves(pos, moves, flags);
	for (int i = 0; i<nm; i++) {
		if (moves[i] == hmove)
			values[i] = MHASH_VALUE;
		else
			values[i] = move_value[flags[i]] + hist_value(moves[i]);
	}
	// more then 1 board
	if (mask2num[pos->validmb] > 1) {
        unsigned int mbthird = (pos->turn == 1) ? third_place[pos->p1vics]&pos->validmb : third_place[pos->p2vics]&pos->validmb;
        if (mbthird) {
            for (int i = 0; i<nm; i++) {
                if (mbthird&(1<<MOVE_MBI(moves[i])))
                    values[i] += 100000;
            }
        }
	}
	// sort by value
	for (int i = 0; i<nm - 1; i++) {
		int maxj = i;
		for (int j = i + 1; j<nm; j++)
			if (values[j] > values[maxj])
				maxj = j;
		if (maxj != i) {
			int tmpv = values[maxj];
			unsigned int tmpm = moves[maxj];
			values[maxj] = values[i];
			moves[maxj] = moves[i];
			values[i] = tmpv;
			moves[i] = tmpm;
		}
	}
	return nm;
}

int victory_minmax(Position *pos, int alpha, int beta, int topd) {
	// game over
	if (pos->status) {
		if (pos->status == 1) return P1VICTORY_VAL - topd;
		if (pos->status == 2) return P2VICTORY_VAL + topd;
		if (pos->status == 3) return DRAW_VAL;
	}

	// static value
	int statv = static_value(pos);

	if (pos->turn == 1) {
		if (statv > alpha) {
			alpha = statv;
			if (alpha >= beta)
				return alpha;
		}
	}
	else {
		if (statv < beta) {
			beta = statv;
			if (alpha >= beta)
				return beta;
		}
	}

	// generate moves
	unsigned int moves[81];
	int nm = victory_moves(pos, moves);
    
    if (nm == 0)
        return statv;

	// victory minmax
	if (pos->turn == 1) {
		for (int i = 0; i<nm; i++) {
			make_move(pos, moves[i]);
			int v = victory_minmax(pos, alpha, beta, topd);
			undo_move(pos, moves[i]);
			if (v > alpha) {
				alpha = v;
				if (alpha >= beta)
					return alpha;
			}
		}
		return alpha;
	}
	else {
		for (int i = 0; i<nm; i++) {
			make_move(pos, moves[i]);
			int v = victory_minmax(pos, alpha, beta, topd);
			undo_move(pos, moves[i]);
			if (v < beta) {
				beta = v;
				if (alpha >= beta)
					return beta;
			}
		}
		return beta;
	}
}

int minmax(Position *pos, int d, int alpha, int beta) {
	runp->nodecnt++;

	// stop control
	if (runp->stop_minmax) return 0;
	if (runp->last_depth > runp->min_depth && runp->nodecnt%NODE_CHECK == 0 && time_now() > runp->stop_time) {
		runp->stop_minmax = 1;
		return 0;
	}
	if (runp->is_pondering && runp->nodecnt%NODE_CHECK == 0 && has_input()) {
		runp->stop_minmax = 1;
		return 0;
	}


    // game over or "quiscence"
	if (pos->status || d <= 0) {
		return victory_minmax(pos, alpha, beta, runp->last_depth - (d + runp->extended));
	}

    // generate and order moves
    unsigned int moves[81], flags[81];
	int nm = sorted_moves(pos, moves, flags);

	// minmax
	if (pos->turn == 1) {
		int ibest = -1;
		for (int i = 0; i<nm; i++) {
			make_move(pos, moves[i]);
            int v = minmax(pos, d - 1, alpha, beta);
			undo_move(pos, moves[i]);
			if (runp->stop_minmax) return 0;
			if (v > alpha) {
                alpha = v;
				ibest = i;
				if (alpha >= beta) {
#ifdef TRIP_STATS
				runp->betatot += i;
				runp->betacnt += 1;
#endif
					insert_ht(pos->hcode, moves[ibest], d, pos->nmove);
					increase_hist(moves[ibest], d);
                    return alpha;
				}
			}
		}
		if (d == runp->last_depth) {
			runp->bestm = moves[ibest];
			runp->bestv = alpha;
		}
		if (ibest >= 0)
			insert_ht(pos->hcode, moves[ibest], d, pos->nmove);
        return alpha;
	}
	else {
		int ibest = -1;
		for (int i = 0; i<nm; i++) {
			make_move(pos, moves[i]);
            int v = minmax(pos, d - 1, alpha, beta);
			undo_move(pos, moves[i]);
			if (runp->stop_minmax) return 0;
			if (v < beta) {
                ibest = i;
				beta = v;
				if (alpha >= beta) {
#ifdef TRIP_STATS
					runp->betatot += i;
					runp->betacnt += 1;
#endif
					insert_ht(pos->hcode, moves[ibest], d, pos->nmove);
					increase_hist(moves[ibest], d);
					return beta;
				}
			}
		}
		if (d == runp->last_depth) {
			runp->bestm = moves[ibest];
			runp->bestv = beta;
		}
		if (ibest >= 0)
			insert_ht(pos->hcode, moves[ibest], d, pos->nmove);
		return beta;
	}
}
