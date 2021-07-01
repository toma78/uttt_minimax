#ifndef BOOK_H
#define BOOK_H
#include <inttypes.h>
#include "position.h"

typedef struct {
	uint64_t hcode;
	uint8_t nmove;
	int32_t value;
} BookEntry;

#define BE_SIZE sizeof(BookEntry)
#define MAX_BOOK_DEPTH 4
//#define RANDOM_VALUE(nmove, limit) (((nmove) < (limit))? (rand()%61-30) : 0)
#define RANDOM_VALUE(nmove, limit) (((nmove) < (limit))? (rand()%6-3) : 0)
//#define BOOK_PATH "./src/mcbook.bin"
//#define BOOK_PATH "./src/tripbook111.bin" // << v112 and v113
//#define BOOK_PATH "./src/tripbook113.bin"
#define BOOK_PATH "./src/tripbook114.bin" // +28 / 500, +56 / 328

// open generated book
void open_book();

// returns 1 if bestm and bestv are set from book
int book_move(Position *pos);

// print initial postion and level 1 of the book
void print_book();

// creates book with evd depth search at highest depth
void create_book(int evd);

#endif

