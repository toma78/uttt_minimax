#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "position.h"

HTEntry *htable = NULL;

int init_ht() {
    htable = (HTEntry*) malloc(sizeof(HTEntry) * HASH_SIZE);
    if(!htable)
        return -1;
    for(int i=0; i<HASH_SIZE; i++) {
        htable[i].d = 0;
        htable[i].m = NOMOVE;
    }        
    return 0;
}

void reset_ht() {
    for(int i=0; i<HASH_SIZE; i++) {
        htable[i].d = 0;
        htable[i].m = NOMOVE;
    }        
}

void insert_ht(uint64_t hc, unsigned int m, int d, unsigned int nmove) {
    int hti = hc&HASH_MASK;
    // replace entry or update entry with higher depth
    if(htable[hti].hc != hc || htable[hti].nmove != nmove || d > htable[hti].d) {
        htable[hti].hc = hc;
        htable[hti].d = (uint8_t)d;
        htable[hti].m = (uint8_t)m;
		htable[hti].nmove = (uint8_t)nmove;
    }
}

unsigned int get_ht_move(uint64_t hc, unsigned int nmove) {
    int hti = hc&HASH_MASK;
    if(htable[hti].hc == hc && htable[hti].nmove == nmove)
        return htable[hti].m;
    else
        return NOMOVE;
}