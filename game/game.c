#include "game.h"

int gprintf(const char *format, ...) {
  if (GUI == 1) {
    va_list args;
    va_start(args, format);
    int status = vprintf(format, args);
    va_end(args);
    return status;
  }
  return 0;
}

int toCoord(int x, int y) {
  if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return -1;
  int coord = x * SIZE + y;
  if (coord >= SIZE * SIZE) return -1;
  else return coord;
}

void initBoard(Board *board) {
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      board->theBoard[toCoord(i, j)] = E;
    }
  }

  board->theBoard[toCoord(3, 3)] = board->theBoard[toCoord(4, 4)] = W;
  board->theBoard[toCoord(3, 4)] = board->theBoard[toCoord(4, 3)] = B;

  board->ws = board->bs = 2;
  board->turn = W;
}

int copyBoard(Board *board, Board **newBoard) {
  if (*newBoard != NULL) return -1;

  Board *tempB = malloc(sizeof(Board));
  if (tempB == NULL) {
    printf("Copy board malloc failed!\n");
    return -1;
  }
  for (int i = 0; i < SIZE * SIZE; ++i) {
    tempB->theBoard[i] = board->theBoard[i];
  }
  tempB->ws = board->ws;
  tempB->bs = board->bs;
  tempB->turn = board->turn;

  *newBoard = tempB;

  return 0;
}

void initPTree(PTree *pt) {
  Board *board = malloc(sizeof(Board));
  if (board == NULL) {
    printf("Malloc failed to allocate board! \n");
  }
  initBoard(board);
  pt->board = board;
  pt->prev = NULL;
  pt->childs = NULL;
  pt->numChilds = 0;
  pt->childLimit = 0;
}

void freePTree(PTree *pt) {
  if (pt == NULL) return;
  free(pt->board);

  for (int i = 0; i < pt->numChilds; ++i) {
    freePTree(pt->childs[i]);
  }
  free(pt);
}

int toCoordWithOffset(int x, int y, int dir, int dist) {
  switch (dir) {
    case 0:
      x += dist;
      break;
    case 1:
      x += dist;
      y += dist;
      break;
    case 2:
      y += dist;
      break;
    case 3:
      x -= dist;
      y += dist;
      break;
    case 4:
      x -= dist;
      break;
    case 5:
      x -= dist;
      y -= dist;
      break;
    case 6:
      y -= dist;
      break;
    case 7:
      x += dist;
      y -= dist;
      break;
    default:
      return -1;
  }
  int coord = toCoord(x, y);
  if (coord >= SIZE * SIZE) return -1;
  else return coord;
}

int moveInDir(Board *board, int x, int y, int dir) {
  int numFlipped = 0;
  int anc = 0;
  for (int dist = 1; dist < SIZE; ++dist) {
    int coord = toCoordWithOffset(x, y, dir, dist);
    if (coord == -1) break;
    
    if (dist == 1 && board->theBoard[coord] == board->turn) break;
    else if (board->theBoard[coord] == board->turn) {
      anc = dist;
      break;
    } else if (board->theBoard[coord] == E) {
      break;
    }
  }
  for (int dist = 1; dist < anc; ++dist) {
    int coord = toCoordWithOffset(x, y, dir, dist);
    board->theBoard[coord] = board->turn;
    numFlipped++;
  }

  return numFlipped;
}

int move(PTree **ptptr, int x, int y) {
  PTree *pt = *ptptr;
  Board *oldBoard = pt->board;
  Board *board = NULL;
  int status = copyBoard(oldBoard, &board);
  if (status != 0) return -1;

  if (board->theBoard[toCoord(x, y)] != E) {
    free(board);
    return 1;
  }

  int numFlipped = 0;
  for (int dir = 0; dir < 8; ++dir) {
    numFlipped += moveInDir(board, x, y, dir);
  }

  if (numFlipped == 0) {
    free(board);
    return 1;
  }

  board->theBoard[toCoord(x, y)] = board->turn;

  if (board->turn == W) {
    board->bs -= numFlipped;
    board->ws += numFlipped + 1;
    board->turn = B;
  } else {
    board->ws -= numFlipped;
    board->bs += numFlipped + 1;
    board->turn = W;
  }


  // create child and point to it instead
  PTree *child = malloc(sizeof(PTree));
  initPTree(child);
  free(child->board);
  child->board = board;
  child->prev = pt;

  if (pt->childs == NULL) {
    pt->childLimit = 1;
    pt->childs = malloc(sizeof(PTree*));
    if (pt->childs == NULL) {
      printf("Childs array first malloc failed!\n");
      return -1;
    }
  } else if (pt->numChilds == pt->childLimit) {
    pt->childLimit *= 2;
    PTree **newChilds = realloc(pt->childs, pt->childLimit * sizeof(PTree*));
    if (newChilds == NULL) {
      printf("Childs array realloc failed!\n");
      return -1;
    }
    pt->childs = newChilds;
  }
  pt->childs[pt->numChilds] = child;
  pt->numChilds++;
  *ptptr = child;

  return 0;
}

int hasMove(PTree *pt) {
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      int moveStatus = move(&pt, i, j);
      if (moveStatus == 0) return 1;
    }
  }
  return 0;
}

void printBoard(Board *board) {
  for (int i = -1; i < SIZE; ++i) {
    for (int j = -1; j < SIZE; ++j) {
      if (i == -1 && j == -1) {
        gprintf(" ");
      } else if (i == -1) {
        gprintf("%d", j);
      } else if (j == -1) { 
        gprintf("%d", i);
      } else {
        enum Square cur = board->theBoard[toCoord(i, j)];
        switch (cur)
        {
        case E:
          gprintf(" ");
          break;
        case W:
          gprintf("W");
          break;
        case B:
          gprintf("B");
          break;
        }
      }
      gprintf(" | ");
    }
    gprintf("\n____________________________________\n");
  }
}
