#ifndef TRIPLEBOT_H
#define TRIPLEBOT_H
#include "position.h"
#include "evaluate.h"

#define PONDER_COMPILE
#define SERVER_DEBUG
//#define TRIP_STATS

#define NODE_CHECK 3000
#define time_now() ((int)(clock()*1000/CLOCKS_PER_SEC))

#define ALPHA -10*P1VICTORY_VAL
#define BETA -10*P2VICTORY_VAL

typedef struct {
    /* external params */
    int tleft, tottime, timeinc;

    /* engine params */
    int max_depth, min_depth;
    int timediv;
    int talk;
    unsigned int rand_limit;
	int do_ponder;
	int use_book;
	int risky;

    /* running params */
    int start_time, stop_time, timeformove, spent_time, stop_minmax;
    int nodecnt;
	int is_pondering;
	int out_of_book;
	int last_depth;
    int extended;

    /* returning params */
    int bestv;
    unsigned int bestm;

    /* stats */
#ifdef TRIP_STATS
    int nodetot, timetot;
    int betatot, betacnt;
    int depthtot, depthcnt;
	int nevals, nprunes, badnprunes;
#endif
} TripParams;

extern TripParams *runp;

int has_input();

void init_bot();

void reset_bot();

void forward_bot();

int best_move(Position *pos);

void eval_pos(Position *pos);

void start_pondering(Position *pos);

void search_report(FILE *fout, int nm, char *msg);

void bot_report(FILE *fout);

#endif
