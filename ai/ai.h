#ifndef AI_H
#define AI_H

#include <math.h>
#include "../game/game.h"

#define SQUARE_WEIGHT { 20, -4, 7, 8, 8, 7, -4, 20, -4, 3, 4, 2, 2, 4, 3, -4, 7, 4, 2, 4, 4, 2, 4, 7, 8, 2, 4, 8, 8, 4, 2, 8, 8, 2, 4, 8, 8, 4, 2, 8, 7, 4, 2, 4, 4, 2, 4, 7, -4, 3, 4, 2, 2, 4, 3, -4, 20, -4, 7, 8, 8, 7, -4, 20 }

static const int square_weights[SIZE * SIZE] = SQUARE_WEIGHT;

int calcEval(PTree *pt, int depth);

int alphaBeta(PTree *pt, int depth, int a, int b);

#endif