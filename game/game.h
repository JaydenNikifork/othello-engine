#ifndef GAME_H
#define GAME_H

#define SIZE 8
#define GUI 0

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
  int eval;
  int bestMove;
} Board;

typedef struct Move {
  int coord;
  int numFlipped;
  int numMoves;
} Move;

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

void initBoard(Board *board);
int copyBoard(Board *board, Board *copied);

int move(Board *board, int x, int y);

int getMoves(Board *b, Move *moves);

int getNumMoves(Board *b);

int hasMove(Board *b);

void printBoard(Board *board);

void initPTree(PTree *pt);

void freePTree(PTree *pt);

int gprintf(const char *format, ...);

#endif