#include <stdio.h>
#include <stdlib.h>
#include "book.h"
#include "triplebot.h"

int book_count = 0;
BookEntry *book;

void open_book() { 
	FILE *fbook = fopen(BOOK_PATH, "rb");
	if (!fbook) {
		fprintf(stderr, "Can't open book!\n");
		return;
	}
	fread(&book_count, sizeof(int), 1, fbook);
	book = (BookEntry*)malloc(sizeof(BookEntry) * book_count);
	for (int i = 0; i < book_count; i++) {
		int r = fread(book + i, BE_SIZE, 1, fbook);
	}
	fclose(fbook);
}

int book_index(Position *pos) {
	for (int i = 0; i < book_count; i++)
		if (pos->hcode == book[i].hcode && pos->nmove == book[i].nmove)
			return i;
	return -1;
}

int book_move(Position *pos) {
	unsigned int moves[81];
	int nm = legal_moves(pos, moves);

	// find move values in book
	int values[81], found = 0;
	for (int i = 0; i < nm; i++) {
		make_move(pos, moves[i]);
		int bind = book_index(pos);
		if (runp->use_book && bind >= 0) { // if book is off use random move 
			values[i] = book[bind].value + RANDOM_VALUE(pos->nmove, runp->rand_limit);
			found++;
		}
		else
            values[i] = RANDOM_VALUE(pos->nmove, runp->rand_limit);
		undo_move(pos, moves[i]);
	}

	if (!found && pos->nmove > 1) { // force one random move without book
		runp->out_of_book = 1;
		return 0;
	}

	// choose randomized best 
	int ibest = 0;
	for (int i = 1; i < nm; i++) {
		if (pos->turn == 1 && values[i] > values[ibest])
			ibest = i;
		if (pos->turn == 2 && values[i] < values[ibest])
			ibest = i;
	}
	runp->bestm = moves[ibest];
	runp->bestv = values[ibest];
	if (runp->talk)
		printf("Book move value = %d\n", runp->bestv);
	return 1;
}

void print_book() {
	Position pos;
	init_pos(&pos, IBSTR, IMBSTR, ITURN, INMOVE);
	printf("init pos value: %d\n", book[book_index(&pos)].value);

	printf("random book lines:\n");
	for (int i = 0; i < 20; i++) {
		init_pos(&pos, IBSTR, IMBSTR, ITURN, INMOVE);
		while (book_move(&pos)) {
			printf("%d. m=%d v=%d ", pos.nmove, mcode2imove(runp->bestm), runp->bestv);
			make_move(&pos, runp->bestm);
		}
		printf("\n");
	}
}

int cwcnt = 0;

int eval_book_pos(Position *pos, int evd) {
	runp->out_of_book = 1;
	runp->max_depth = evd;
	runp->tleft = 1000000;
	best_move(pos);
	return runp->bestv;
}

int create_walk(FILE *fbook, Position *pos, int evd) {
	cwcnt++;
	if (cwcnt % 100 == 0)
		printf("Creating book %d / 62218\n", cwcnt);

	if (pos->nmove == MAX_BOOK_DEPTH) {
		BookEntry be;
		be.hcode = pos->hcode;
		be.nmove = pos->nmove;
		be.value = eval_book_pos(pos, evd);
		int r = fwrite(&be, BE_SIZE, 1, fbook);
		return be.value;
	}

	unsigned int moves[81];
	int nm = legal_moves(pos, moves);

	int bestv;
	if (pos->turn == 1) {
		bestv = ALPHA;
		for (int i = 0; i < nm; i++) {
			make_move(pos, moves[i]);
			int v = create_walk(fbook, pos, evd);
			undo_move(pos, moves[i]);
			if (v > bestv)
				bestv = v;
		}
	}
	else {
		bestv = BETA;
		for (int i = 0; i < nm; i++) {
			make_move(pos, moves[i]);
			int v = create_walk(fbook, pos, evd);
			undo_move(pos, moves[i]);
			if (v < bestv)
				bestv = v;
		}
	}

	BookEntry be;
	be.hcode = pos->hcode;
	be.nmove = pos->nmove;
	be.value = bestv;
	int r = fwrite(&be, BE_SIZE, 1, fbook);
	return bestv;
}

void create_book(int evd) {
	FILE *fbook = fopen(BOOK_PATH, "wb");
	if (!fbook) {
		fprintf(stderr, "Can't open book!\n");
		return;
	}

	int book_size = 62218;
	fwrite(&book_size, sizeof(int), 1, fbook);
	Position pos;
	init_pos(&pos, IBSTR, IMBSTR, ITURN, INMOVE);

	cwcnt = 0;
	create_walk(fbook, &pos, evd);
	printf("Done %d!\n", cwcnt);
	fclose(fbook);
}
