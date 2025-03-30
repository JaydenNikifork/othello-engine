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

  board->eval = 0;
  board->bestMove = -1;
}

int copyBoard(Board *board, Board *copied) {
  // for (int i = 0; i < SIZE * SIZE; ++i) {
  //   copied->theBoard[i] = board->theBoard[i];
  // }
  memcpy(copied->theBoard, board->theBoard, SIZE * SIZE * sizeof(Square));
  copied->ws = board->ws;
  copied->bs = board->bs;
  copied->turn = board->turn;

  copied->eval = board->eval;
  copied->bestMove = board->bestMove;

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

int move(Board *board, int x, int y) {
  if (board->theBoard[toCoord(x, y)] != E) {
    return 0;
  }

  int numFlipped = 0;
  for (int dir = 0; dir < 8; ++dir) {
    numFlipped += moveInDir(board, x, y, dir);
  }

  if (numFlipped == 0) {
    return 0;
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

  return numFlipped;
}

int getMoves(Board *b, Move *moves) {
  // assumes moves is allocated to hold SIZE * SIZE moves
  Board scrapB;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      copyBoard(b, &scrapB);
      int numFlipped = move(&scrapB, i, j);
      int coord = toCoord(i, j);
      if (numFlipped == 0) {
        moves[coord].coord = -1;
        moves[coord].numFlipped = 0;
        moves[coord].numMoves = 0;
      } else {
        int numMoves = getNumMoves(&scrapB);
        moves[coord].coord = coord;
        moves[coord].numFlipped = numFlipped;
        moves[coord].numMoves = numMoves;
      }
    }
  }
}

int getNumMoves(Board *b) {
  Move moves[SIZE * SIZE];
  Board scrapB;
  int numMoves = 0;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      copyBoard(b, &scrapB);
      int numFlipped = move(&scrapB, i, j);
      if (numFlipped > 0) {
        ++numMoves;
      }
    }
  }
  return numMoves;
}

int hasMove(Board *b) {
  Board copied;
  copyBoard(b, &copied);
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      int numFlipped = move(&copied, i, j);
      if (numFlipped != 0) return 1;
    }
  }
  return 0;
}

void printState(Board *b) {
  char str[SIZE * SIZE + 3];
  str[0] = '0' + b->turn;
  str[1] = '-';
  for (int i = 0; i < SIZE * SIZE; ++i) {
    str[i + 2] = '0' + b->theBoard[i];
  }
  str[SIZE * SIZE + 2] = '\0';
  printf("%s\n", str);
  fflush(stdout);
}

void printBoard(Board *board) {
  if (GUI) {
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
  } else {
    printState(board);
  }
}
