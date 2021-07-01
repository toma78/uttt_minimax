#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "position.h"
#include "triplebot.h"
#include "book.h"

#ifdef TRIP_STATS
extern int code_value[21];
#endif

// gcc -std=c99 -O2 -static ./src/*.c -o trip.exe
int main() {
	Position pos;
	setbuf(stdout, 0);
	srand((unsigned int)time(0));
	init_bot();
	int turn = 0, nmove = 0;
	char *ptr=0, instr[4096], field[1024], mbfield[1024];
	while(1) {
		ptr = fgets(instr, 4096, stdin);
		if(!strncmp("action move", instr, 11)) {
			ptr=0;
			runp->tleft = strtol(instr+12, &ptr, 10);
			init_pos(&pos, field, mbfield, turn, nmove);
			int bm = best_move(&pos);
            printf("place_move %d %d\n", bm%9, bm/9);
			forward_bot();
			start_pondering(&pos);
		} else if(!strncmp("update game field", instr, 17)) {
			strcpy(field, instr+18);
		} else if(!strncmp("update game macroboard", instr, 22)) {
			strcpy(mbfield, instr+23);
		} else if(!strncmp("update game move", instr, 16)) {
            ptr=0;
			nmove = strtol(instr+17, &ptr, 10) - 1; // start from 0!
		} else if(!strncmp("settings timebank", instr, 17)) {
            ptr=0;
			runp->tottime = strtol(instr+18, &ptr, 10);
		} else if(!strncmp("settings time_per_move", instr, 22)) {
            ptr=0;
			runp->timeinc = strtol(instr+23, &ptr, 10);
		} else if(!strncmp("settings your_botid", instr, 19)) {
            ptr=0;
			turn = strtol(instr+20, &ptr, 10);
		} else if(!strncmp("talk", instr, 4)) {
			ptr = 0;
			runp->talk = strtol(instr + 5, &ptr, 10);;
			if (runp->talk)
				printf("starting to talk!\n");
		} else if(!strncmp("reset bot", instr, 9)) {
			printf("reseting bot!\n");
			reset_bot();
		} else if(!strncmp("max depth", instr, 9)) {
            ptr=0;
			runp->max_depth = strtol(instr+10, &ptr, 10);
			printf("setting max depth to %d!\n", runp->max_depth);
			if (runp->max_depth < runp->min_depth)
				runp->min_depth = runp->max_depth;
		} else if (!strncmp("ponder", instr, 6)) {
			ptr = 0;
			runp->do_ponder = strtol(instr + 7, &ptr, 10);
			printf("setting ponder to %d!\n", runp->do_ponder);
		} else if(!strncmp("print stats", instr, 11)) {
            #ifdef TRIP_STATS
			bot_report(stdout);
            #else
            printf("no stats!\n");
            #endif
		} else if(!strncmp("exit", instr, 4)) {
		    break;
		} else if(!strncmp("rand limit", instr, 10)) {
            ptr=0;
			runp->rand_limit = strtol(instr+11, &ptr, 10);
			printf("setting random limit to %d!\n", runp->rand_limit);
		} else if (!strncmp("print book", instr, 10)) {
			print_book();
		} else if (!strncmp("book", instr, 4)) {
			ptr = 0;
			runp->use_book = strtol(instr + 5, &ptr, 10);
			printf("using book %d!\n", runp->use_book);
		} else if (!strncmp("risky", instr, 5)) {
			ptr = 0;
			runp->risky = strtol(instr + 6, &ptr, 10);
		} else if (!strncmp("eval pos", instr, 8)) {
			init_pos(&pos, field, mbfield, turn, nmove);
			eval_pos(&pos);
		}
		else if (!strncmp("triples", instr, 7)) {
			init_pos(&pos, field, mbfield, turn, nmove);
			print_triples(&pos);
		}
		else if(!strncmp("factors ", instr, 7)) {
            #ifdef TRIP_STATS
            ptr = instr + 8;
            for(int c=0; c<21; c++)
                code_value[c] = strtol(ptr, &ptr, 10);
            #endif
        } else if(!strncmp("ordfact ", instr, 7)) {
            #ifdef TRIP_STATS
            //ptr = instr + 8;            
            //for(int c=1; c<5; c++) // ?????
            //    ord_factors[c] = strtol(ptr, &ptr, 10);
            #endif
        } else if (!strncmp("create book", instr, 11)) {
			ptr = 0;
			int evd = strtol(instr + 12, &ptr, 10);
			create_book(evd);
		}
	}
	return 0;
}
