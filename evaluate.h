#ifndef EVALUATE_H
#define EVALUATE_H
#include "position.h"

void init_cache();

int static_value(Position *pos);

void elaborate_static_value(Position *pos);

void print_triples(Position *pos);

#endif

