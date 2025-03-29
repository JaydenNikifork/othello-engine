#include "ai.h"

int getSquareScore(Board *b, int x, int y) {
  int coord = toCoord(x, y);
  Square col = b->theBoard[coord];
  if (col == E) return 0;

  int score = 0;
  int colMul = (col == W) * 2 - 1;

  score += colMul * square_weights[coord];

  // check if contained
  if (
    (x == 0 || b->theBoard[toCoord(x-1, y)] != col) &&
    (x == SIZE - 1 || b->theBoard[toCoord(x+1, y)] != col) &&
    (y == 0 || b->theBoard[toCoord(x, y-1)] != col) &&
    (y == SIZE - 1 || b->theBoard[toCoord(x, y+1)] != col)
  ) {
    score += colMul * 5;
  } else {
    score += colMul * -1;
  }
}

int getBoardScore(Board *b) {
  int score = 0;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      score += getSquareScore(b, i, j);
    }
  }
  return score;
}

int alphaBeta(PTree *pt, int depth, int a, int b) {
  if (depth == 0) return getBoardScore(pt->board);

  Square turn = pt->board->turn;
  int value;
  if (turn == W) {
    value = -1000000;
    for (int i = 0; i < SIZE; ++i) {
      int doBreak = 0;
      for (int j = 0; j < SIZE; ++j) {
        int moveStatus = move(&pt, i, j);
        if (moveStatus != 0) continue;
        int moveEval = alphaBeta(pt, depth - 1, a, b);
        pt = pt->prev;
        if (moveEval > value) {
          value = moveEval;
          pt->bestMove = toCoord(i, j);
          pt->eval = value;
        }
        if (value >= b) {
          doBreak = 1;
          break;
        }
        if (value > a) a = value;
      }
      if (doBreak) break;
    }
    return value;
  } else {
    value = 1000000;
    for (int i = 0; i < SIZE; ++i) {
      int doBreak = 0;
      for (int j = 0; j < SIZE; ++j) {
        int moveStatus = move(&pt, i, j);
        if (moveStatus != 0) continue;
        int moveEval = alphaBeta(pt, depth - 1, a, b);
        pt = pt->prev;
        if (moveEval < value) {
          value = moveEval;
          pt->bestMove = toCoord(i, j);
          pt->eval = value;
        }
        if (value <= a) {
          doBreak = 1;
          break;
        }
        if (value > b) b = value;
      }
      if (doBreak) break;
    }
    return value;
  }
}

int calcEval(PTree *pt, int depth) {
  if (depth < 0) return -1;
  
  if (depth == 0) {
    pt->eval = getBoardScore(pt->board);
    return 0;
  }

  Square turn = pt->board->turn;
  int bestScore = 0;
  if (turn == W) bestScore = -100000;
  if (turn == B) bestScore = 100000;
  int bestMove = -1;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      int moveStatus = move(&pt, i, j);
      if (moveStatus == 0) {
        calcEval(pt, depth - 1);
        if (turn == W) {
          if (pt->eval > bestScore) {
            bestMove = toCoord(i, j);
            bestScore = pt->eval;
          }
        } else {
          if (pt->eval < bestScore) {
            bestMove = toCoord(i, j);
            bestScore = pt->eval;
          }
        }
        pt = pt->prev;
        freePTree(pt->childs[--pt->numChilds]);
        pt->childs[pt->numChilds] = NULL;
      }
    }
  }

  pt->eval = bestScore;
  pt->bestMove = bestMove;

  return 0;
}