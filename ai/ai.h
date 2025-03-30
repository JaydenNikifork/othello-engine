#ifndef AI_H
#define AI_H

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../game/game.h"

#define SQUARE_WEIGHT { 30, -15, 7, 8, 8, 7, -15, 30, -15, 3, 4, 2, 2, 4, 3, -15, 7, 4, 2, 4, 4, 2, 4, 7, 8, 2, 4, 8, 8, 4, 2, 8, 8, 2, 4, 8, 8, 4, 2, 8, 7, 4, 2, 4, 4, 2, 4, 7, -15, 3, 4, 2, 2, 4, 3, -15, 30, -15, 7, 8, 8, 7, -15, 30 }
#define TERMINAL 10000000
#define DEPTH 11
#define ODDS 6
#define NUM_RUNS 3 
#define AVG_MOVES 10

static const int square_weights[SIZE * SIZE] = SQUARE_WEIGHT;

int calcEval(PTree *pt, int depth);

int alphaBeta(Board *board, int depth, int a, int b);

#endif