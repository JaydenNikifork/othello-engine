#include "game.h"
#include "../ai/ai.h"

int main() {
  PTree *root = malloc(sizeof(PTree));  // keep root for free
  initPTree(root);
  PTree *pt = root;

  gprintf("Game start! \n");

  int noMovesPrev = 0;
  while (pt->board->ws + pt->board->bs < SIZE * SIZE) {
    printBoard(pt->board);

    int hasMoves = hasMove(pt);
    if (!hasMove) {
      if (noMovesPrev) {
        break;
      } else {
        noMovesPrev = 1;
      }
    } else {
      noMovesPrev = 0;
    }

    alphaBeta(pt, 7, -1000000, 1000000);

    gprintf("Eval: %d \n", pt->eval);

    int x, y;
    if (pt->board->turn == W) {
      gprintf("White's turn! \n");
    } else {
      gprintf("Black's turn! \n");
      x = pt->bestMove / SIZE;
      y = pt->bestMove % SIZE;
      move(&pt, x, y);
      continue;
    }

    gprintf("Enter x: \n");
    scanf("%d", &y);
    gprintf("Enter y: \n");
    scanf("%d", &x);
    int moveStatus = move(&pt, x, y);
    if (moveStatus > 0) {
      gprintf("Invalid move, try again. \n");
    } else if (moveStatus < 0) {
      printf("Fatal error! Code: %d \n", moveStatus);
      return 1;
    }
  }
  
  printBoard(pt->board);

  if (pt->board->ws > pt->board->bs) {
    gprintf("White wins! %d-%d\n", pt->board->ws, pt->board->bs);
  } else if (pt->board->ws < pt->board->bs) {
    gprintf("Black wins! %d-%d\n", pt->board->bs, pt->board->ws);
  } else {
    gprintf("It's a draw! \n");
  }

  freePTree(root);

  return 0;
}