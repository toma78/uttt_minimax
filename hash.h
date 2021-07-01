#ifndef HASH_H
#define HASH_H
#include <inttypes.h>

#define HASH_SIZE (1 << 23)
#define HASH_MASK (HASH_SIZE-1)

typedef struct {
	uint64_t hc; // hash code
	uint8_t nmove; // move number
	uint8_t m; // move
	uint8_t d; // depth
} HTEntry;

int init_ht();

void reset_ht();

void insert_ht(uint64_t hc, unsigned int m, int d, unsigned int nmove);

unsigned int get_ht_move(uint64_t hc, unsigned int nmove);

#endif
