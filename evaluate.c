#include <stdio.h>
#include "evaluate.h"
#include "triplebot.h"

#define triple_code(f,s,t) (((f)<<6)|((s)<<3)|(t))

const int triples[8][3] = { {0, 3, 6}, {0, 1, 2}, {0, 4, 8}, {2, 4, 6}, {1, 4, 7}, {3, 4, 5}, {2, 5, 8}, {6, 7, 8} };

#define DDD 0
#define ODD 0
#define OOD 0
#define PDD 0
#define POD 0
#define PPD 0
#define TDD 0
#define TOD 0
#define TPD 0
#define TTD 0
#define VDD 0
#define VOD 0
#define VPD 0
#define VTD 0
#define VVD 0

#define OOO 1
#define POO 2
#define PPO 3
#define PPP 4
#define TOO 5
#define TPO 6
#define TPP 7
#define TTO 8
#define TTP 9
#define TTT 10
#define VOO 11
#define VPO 12
#define VPP 13
#define VTO 14
#define VTP 15
#define VTT 16
#define VVO 17
#define VVP 18
#define VVT 19
#define VVV 20

const int code_index[512] = {
	DDD, ODD, PDD, TDD, VDD, 0, 0, 0, ODD, OOD, POD, TOD, VOD, 0, 0, 0, PDD, POD, PPD, TPD, VPD, 0, 0, 0, TDD, TOD, TPD, TTD, VTD, 0, 0, 0, 
	VDD, VOD, VPD, VTD, VVD, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ODD, OOD, POD, TOD, VOD, 0, 0, 
	0, OOD, OOO, POO, TOO, VOO, 0, 0, 0, POD, POO, PPO, TPO, VPO, 0, 0, 0, TOD, TOO, TPO, TTO, VTO, 0, 0, 0, VOD, VOO, VPO, VTO, VVO, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, PDD, POD, PPD, TPD, VPD, 0, 0, 0, POD, POO, PPO, TPO, VPO, 0, 
	0, 0, PPD, PPO, PPP, TPP, VPP, 0, 0, 0, TPD, TPO, TPP, TTP, VTP, 0, 0, 0, VPD, VPO, VPP, VTP, VVP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TDD, TOD, TPD, TTD, VTD, 0, 0, 0, TOD, TOO, TPO, TTO, VTO, 0, 0, 0, TPD, TPO, TPP, TTP, VTP, 0, 
	0, 0, TTD, TTO, TTP, TTT, VTT, 0, 0, 0, VTD, VTO, VTP, VTT, VVT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, VDD, VOD, VPD, VTD, VVD, 0, 0, 0, VOD, VOO, VPO, VTO, VVO, 0, 0, 0, VPD, VPO, VPP, VTP, VVP, 0, 0, 0, VTD, VTO, VTP, VTT, VVT, 0, 
	0, 0, VVD, VVO, VVP, VVT, VVV, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

#ifdef TRIP_STATS
// triple values       DDD OOO  POO  PPO  PPP  TOO  TPO  TPP   TTO   TTP   TTT  VOO   VPO   VPP   VTO   VTP   VTT   VVO   VVP   VVT  VVV
//v112 int code_value[21] = { 0,  5,  10,  20,  35,  20,  40,  65,   70,  110,  190,  40,   80,  130,  135,  215,  355,  215,  370,  635, 1200 };
//v113 int code_value[21] = {   0, 24,  31,  40,  55,  40,  55,  78,   79,  121,  196,  56,   85,  130,  138,  222,  370,  231,  403,  646, 1000 };
// // shorts! tot [1000, 239, 61, 700] int code_value[21] = {   0,  0,   1,   1,   3,   1,   3,   7,    7,   17,   43,   3,    8,   21,   18,   57,  146,   45,  162,  420, 1000 };

// GOOD! ./trip.elf vs ./trip112.elf 524 : 475 diff 49
int code_value[21] = {   0, 24,  31,  40,  55,  40,  55,  78,   79,  121,  196,  56,   85,  130,  138,  222,  370,  231,  403,  646, 1000 };
//  int code_value[21] = {   0, 24,  31,  41,  55,  40,  55,  78,   79,  122,  198,  56,   85,  130,  138,  223,  372,  232,  404,  648, 1000 };
//int code_value[21] = {   0, 24,  31,  40,  55,  40,  55,  78,   78,  122,  198,  55,   85,  130,  138,  223,  372,  232,  404,  648, 1000 };
//int code_value[21] = {                               0, 25, 30, 40, 55, 40, 55, 80, 80, 120, 195, 55, 85, 130, 140, 220, 370, 230, 405, 645, 1000 };
/*
a:0 5 10 20 35 20 40 65 70 110 190 40 80 130 135 215 355 215 370 635 1200 q(a):0.634445843829 n(a):794
a:0 0 1 1 3 1 3 7 7 17 43 3 8 21 18 57 146 45 162 420 1000 q(a):0.602443609023 n(a):532
a:0 1 1 2 4 2 5 10 10 24 55 4 11 29 26 72 172 58 191 454 1000 q(a):0.630651595745 n(a):752
a:0 0 0 1 2 1 2 5 5 13 34 2 5 15 13 45 125 35 139 390 1000 q(a):0.630651595745 n(a):752

a:0 5 10 20 35 20 40 65 70 110 190 40 80 130 135 215 355 215 370 635 1200 q(a):0.664133738602 n(a):329
a:0 24 31 40 55 40 55 78 79 121 196 56 85 130 138 222 370 231 403 646 1000 q(a):0.696921443737 n(a):471
a:0 25 30 40 55 40 55 80 80 120 195 55 85 130 140 220 370 230 405 645 1000 q(a):0.636363636364 n(a):253

 * */
#else
// triple values             DDD OOO  POO  PPO  PPP  TOO  TPO  TPP   TTO   TTP   TTT  VOO   VPO   VPP   VTO   VTP   VTT   VVO   VVP   VVT    VVV
const int code_value[21] = { 0, 24,  31,  40,  55,  40,  55,  78,   79,  121,  196,  56,   85,  130,  138,  222,  370,  231,  403,  646, 1000 };
#endif	

static char board_cache[512][512];

int board_status(unsigned int board, unsigned int nboard) {
	// board over or obviously illegal
	if (is_vic(board) && is_vic(nboard))
		return -1;
	if (is_vic(board) && !is_vic(nboard))
		return VCODE;
	if (is_vic(nboard) && !is_vic(board))
		return DCODE;
	if (is_full(board|nboard))
		return DCODE;
	
	// evaluate board
	int presence = 0, threat = 0, blocks = 0;
    for(int i=0; i<8; i++) {
        unsigned int trip = (1<<triples[i][0])|(1<<triples[i][1])|(1<<triples[i][2]);
        if (nboard&trip) {
            blocks++;
        }
        else if(mask2num[board&trip] == 1) {
            presence = 1;
        } 
        else if(mask2num[board&trip] == 2) {
            threat = 1;
        }
    }
        
	int bcode;
    if(threat)
        bcode = TCODE;
    else if(blocks==8)
        bcode = DCODE;
	else if(presence)
        bcode = PCODE;
    else
        bcode = OCODE;
	
	return bcode;
}

void init_cache() {
	for (int board = 0; board<512; board++)
		for (int nboard = 0; nboard<512; nboard++)
			board_cache[board][nboard] = board_status(board, nboard);
}

#define p1code(bi) board_cache[pos->p1boards[bi]][pos->p2boards[bi]] 
#define p2code(bi) board_cache[pos->p2boards[bi]][pos->p1boards[bi]] 

int static_value(Position *pos) {
#ifdef TRIP_STATS
	runp->nevals++;
#endif
    
    int p1tval = 0, p2tval = 0;
    int p1tmax = 0, p2tmax = 0;
    // evaluate each triple
    for (int i = 0; i<8; i++) {
        int f = triples[i][0], s = triples[i][1], t = triples[i][2];
        int p1tc = triple_code(p1code(f), p1code(s), p1code(t));
        int c1val = code_value[code_index[p1tc]];
        if(c1val > p1tmax)
            p1tmax = c1val;
        p1tval += c1val;
        int p2tc = triple_code(p2code(f), p2code(s), p2code(t));
        int c2val = code_value[code_index[p2tc]];
        if(c2val > p2tmax)
            p2tmax = c2val;
        p2tval += c2val;
    }                
    
    //return (p1tval - p2tval);
    return 2 * (p1tval - p2tval) + (p1tmax - p2tmax);  // <<< 112
    //return (p1tmax - p2tmax);
    
// YES! ./trip.elf vs ./trip111.elf 547 : 453 diff 94

    /*
8 * sum + a * max
a:8 q(a):0.629761904762 n(a):420
a:12 q(a):0.636761487965 n(a):457
a:2 q(a):0.622795969773 n(a):397
a:4 q(a):0.643939393939 n(a):495
a:16 q(a):0.640918580376 n(a):479
*/
    
/* sum + a * max (new values)
 *a:1 q(a):0.641497461929 n(a):394
a:0 q(a):0.609589041096 n(a):292
a:3 q(a):0.632271468144 n(a):361
a:2 q(a):0.632920110193 n(a):363
a:4 q(a):0.630311614731 n(a):353
*/    
}

void print_triples(Position *pos) {
#ifdef TRIP_STATS
	runp->nevals++;
#endif

	// print each triple
	int p1indexes[8], p2indexes[8];
	for (int i = 0; i<8; i++) {
		int f = triples[i][0], s = triples[i][1], t = triples[i][2];
		p1indexes[i] = code_index[triple_code(p1code(f), p1code(s), p1code(t))];
		p2indexes[i] = code_index[triple_code(p2code(f), p2code(s), p2code(t))];
	}
	printf("{ 'p1': [ ");
	for (int i = 0; i<8; i++)
		printf("%d,", p1indexes[i]);
	printf("], 'p2': [ ");
	for (int i = 0; i<8; i++)
		printf("%d,", p2indexes[i]);
	printf("] }\n");
}

void elaborate_static_value(Position *pos) {
#ifdef TRIP_STATS
	FILE *fd = stdout;
	const char n2codes[5] = { 'D', 'O', 'P', 'T', 'V' };
	
	fprintf(fd, "P1 codes\n");
    for(int bi=0; bi<9; bi++) {
		fprintf(fd, "%c", n2codes[p1code(bi)]);
		if((bi+1)%3 == 0)
			fprintf(fd, "\n");
	}
	fprintf(fd, "P2 codes\n");
    for(int bi=0; bi<9; bi++) {
		fprintf(fd, "%c", n2codes[p2code(bi)]);
		if((bi+1)%3 == 0)
			fprintf(fd, "\n");
	}	
	

	fprintf(fd, "tcodes: ");
	for(int i=0; i<8; i++) {
		int f = triples[i][0], s = triples[i][1], t = triples[i][2];
		fprintf(fd, "%c%c%c,", n2codes[p1code(f)], n2codes[p1code(s)], n2codes[p1code(t)]);
	}
	for(int i=0; i<8; i++) {
		int f = triples[i][0], s = triples[i][1], t = triples[i][2];
		fprintf(fd, "%c%c%c,", n2codes[p2code(f)], n2codes[p2code(s)], n2codes[p2code(t)]);
	}
	fprintf(fd, "\n");

    
	fprintf(fd, "boards: ");
    for(int bi=0; bi<9; bi++) {
		for(int i=0; i<9; i++) {
			if( (pos->p1boards[bi]&(1<<i)) && (pos->p2boards[bi]&(1<<i))) {
				fprintf(fd, "nooooo,\n");				
			}
			else if(pos->p1boards[bi]&(1<<i))
				fprintf(fd, "1,");
			else if(pos->p2boards[bi]&(1<<i))
				fprintf(fd, "-1,");
			else
				fprintf(fd, "0,");
		}
	}
	fprintf(fd, "\n");
	
	#endif
}
