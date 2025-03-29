#ifndef GAME_H
#define GAME_H

#define SIZE 8
#define GUI 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef enum Square {
  E,
  W,
  B
} Square;

typedef struct Board {
  Square theBoard[SIZE * SIZE];
  int ws, bs; // whites and blacks placed
  Square turn;
} Board;

typedef struct PTree {
  Board *board;
  struct PTree *prev;
  struct PTree **childs;
  int childLimit;
  int numChilds;
  int eval;
  int bestMove;
} PTree;

typedef struct Game {
  PTree *posTree;
} Game;

int toCoord(int x, int y);

int move(PTree **ptptr, int x, int y);

int hasMove(PTree *pt);

void printBoard(Board *board);

void initPTree(PTree *pt);

void freePTree(PTree *pt);

int gprintf(const char *format, ...);

#endif