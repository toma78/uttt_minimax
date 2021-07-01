#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "triplebot.h"
#include "evaluate.h"
#include "minimax.h"
#include "hash.h"
#include "book.h"

TripParams params = {
	.tleft = 0, .tottime = 0, .timeinc = 500, .timeformove = 0,
	.min_depth = 4,.max_depth = 32,.timediv = 7,.talk = 0,
	.risky = 0,.use_book = 1, .out_of_book = 0,
	.rand_limit = 3, .is_pondering = 0, .extended = 0,
	#ifdef PONDER_COMPILE
	.do_ponder = 1,
	#else
	.do_ponder = 0,
	#endif	
	#ifdef TRIP_STATS
	.nodetot = 0, .timetot = 1,
	.betatot = 0, .betacnt = 1,
	.depthtot = 0, .depthcnt = 1,
	.nevals = 0, .nprunes = 0, .badnprunes = 0
	#endif
};
TripParams *runp = &params;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>
#include <wincon.h>
int has_input() {
	DWORD ne;
	HANDLE sin = GetStdHandle(STD_INPUT_HANDLE);
	GetNumberOfConsoleInputEvents(sin, &ne);
	return (ne > 0);
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
int has_input() {
	fd_set readfds;
	FD_ZERO(&readfds);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_SET(STDIN_FILENO, &readfds);

	return select(1, &readfds, NULL, NULL, &timeout);
}
#endif

void init_bot() {
	init_ht();
	open_book();
	reset_history();
	init_cache();
}

void reset_bot() {
    reset_ht();
    reset_history();
	runp->out_of_book = 0;
	runp->last_depth = 0;
	runp->bestm = NOMOVE;
	runp->bestv = 0;
#ifdef TRIP_STATS
	runp->nodetot = 0; runp->timetot = 1;
	runp->betatot = 0; runp->betacnt = 1;
    runp->depthtot = 0; runp->depthcnt = 1;
	runp->nevals = 0;
	runp->nprunes = 0;
    runp->badnprunes = 0;
#endif
}

void forward_bot() {
	age_history();
}

int search(Position *pos) {
    runp->stop_minmax = 0;
	runp->last_depth = 1;
	while(runp->last_depth <= runp->max_depth) {
        	minmax(pos, runp->last_depth, ALPHA, BETA);
        runp->spent_time = (1 + time_now() - runp->start_time);
		if(runp->talk) {			
			char *fs = "d=%2d v=%6d move=(%d,%d) ncnt=%10d time=%8dms nps=%8dK\n";
			printf(fs, runp->last_depth, runp->bestv, mcode2imove(runp->bestm)%9, mcode2imove(runp->bestm)/9, runp->nodecnt, runp->spent_time, runp->nodecnt/runp->spent_time);
		}
		if(runp->stop_minmax && runp->last_depth >= runp->min_depth) {
            return 1;
        } else if(runp->spent_time > runp->timeformove/2  && runp->last_depth >= runp->min_depth) {
            return 2;
        } else if(runp->bestv > (P1VICTORY_VAL-100) || (P2VICTORY_VAL+100) > runp->bestv) {
            return 3;
        }
		
        runp->last_depth++;
    }
	return 4;	
}

void allocate_time(int nmove) {
    runp->start_time = time_now();
    int alltime = 0;
    int delta = (runp->tleft - runp->timeinc) / 100;    
    //const int opening = 16, peak = 28;
    const int opening = 16, peak = 32;
    if (nmove < opening)	
        alltime = runp->timeinc + delta;
    else if (nmove < peak)
        alltime = runp->timeinc + 2 * (1+nmove-opening) * delta;
    else
        alltime = runp->timeinc + 33 * delta;
	runp->stop_time = runp->start_time + alltime;
	runp->timeformove = runp->stop_time - runp->start_time;	
	if (runp->talk)
		printf("Move %d, tinc %d, tleft %d, time for move %d\n", nmove, runp->timeinc, runp->tleft, runp->timeformove);
}

int best_move(Position *pos) {
    runp->nodecnt = 0;

	// try random / book
	if (!runp->out_of_book && book_move(pos)) {
		#ifdef SERVER_DEBUG
			fprintf(stderr, "Book %2d v=%d move=(%d,%d)\n", pos->nmove, runp->bestv, mcode2imove(runp->bestm)%9, mcode2imove(runp->bestm)/9);
		#endif
		runp->last_depth = 0;
		return mcode2imove(runp->bestm);
	}

	// timed search
	allocate_time(pos->nmove);
	int rez = search(pos);
	
	if(runp->talk) {
		search_report(stdout, pos->nmove, "think");
	}

	#ifdef SERVER_DEBUG
		search_report(stderr, pos->nmove, "think");
	#endif

    #ifdef TRIP_STATS
        runp->nodetot += runp->nodecnt;
        runp->timetot += runp->spent_time;
        runp->depthtot += runp->last_depth;
        runp->depthcnt += 1;
    #endif

	return mcode2imove(runp->bestm);
}

void eval_pos(Position *pos) {
    printf("%d\n", static_value(pos));
    /*
	print_pos(pos);
	elaborate_static_value(pos);
	*/
}

void ponder(Position *pos) {
	runp->nodecnt = 0;

	runp->start_time = time_now();
	runp->stop_time = runp->start_time + 600000; // 10 mins
	runp->timeformove = runp->stop_time - runp->start_time;
	if (runp->talk)
		printf("Time for pondering %d\n", runp->timeformove);
	runp->stop_minmax = 0;
	runp->is_pondering = 1;

	runp->last_depth = (runp->last_depth < 2)? 1 : runp->last_depth - 1; // continue where search stopped
	while (runp->last_depth <= runp->max_depth) {
		minmax(pos, runp->last_depth, ALPHA, BETA);
		runp->spent_time = (1 + time_now() - runp->start_time);
		if (runp->stop_minmax) {
			if (runp->talk)
				printf("Ponder stopped! Total time spent %d\n", runp->spent_time);
			break;
		}
		if (runp->talk)
			search_report(stdout, pos->nmove, "ponder");	
		if (runp->bestv > (P1VICTORY_VAL-100) || (P2VICTORY_VAL+100) > runp->bestv) {
			if (runp->talk)
				printf("Game over\n");
			break;
		}

		runp->last_depth++;
	}

#ifdef SERVER_DEBUG
	search_report(stderr, pos->nmove, "ponder");
#endif

	runp->is_pondering = 0;

#ifdef TRIP_STATS
	runp->nodetot += runp->nodecnt;
	runp->timetot += runp->spent_time;
	runp->depthtot += runp->last_depth;
	runp->depthcnt += 1;
#endif
}

void start_pondering(Position *pos) {
	if (runp->do_ponder && pos->status == 0) {
		make_move(pos, runp->bestm);
		if (pos->status == 0) {
			ponder(pos);
		}
	}
}

void search_report(FILE *fout, int nm, char *msg) {
	char *fs = "%2d (%s) d=%2d v=%6d move=(%d,%d) ncnt=%10d time=%8dms nps=%8dK\n";
	fprintf(fout, fs, nm, msg, runp->last_depth, runp->bestv,
		mcode2imove(runp->bestm) % 9, mcode2imove(runp->bestm) / 9,
		runp->nodecnt, runp->spent_time, runp->nodecnt / runp->spent_time);
}

void bot_report(FILE *fout) {
#ifdef TRIP_STATS
	fprintf(fout, "{ 'ncount':%d, 'neval':%d, 'nprune':%d, 'badnprune':%d, 'avg_depth':%lf, 'avg_bcut':%lf, 'avg_knps':%lf }\n",
		runp->nodetot, runp->nevals, runp->nprunes, runp->badnprunes, ((double)runp->depthtot) / runp->depthcnt,
		((double)runp->betatot) / runp->betacnt, ((double)runp->nodetot) / runp->timetot);
#endif
}

