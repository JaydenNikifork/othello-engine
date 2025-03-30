#include "game.h"
#include "../ai/ai.h"

int main() {
  Board b;
  initBoard(&b);

  gprintf("Game start! \n");

  int noMovesPrev = 0;
  while (b.ws + b.bs < SIZE * SIZE) {
    printBoard(&b);

    int hasMoves = hasMove(&b);
    if (!hasMoves) {
      if (noMovesPrev) {
        break;
      } else {
        noMovesPrev = 1;
        if (b.turn == W) b.turn = B;
        else b.turn = W;
        continue;
      }
    } else {
      noMovesPrev = 0;
    }

    alphaBeta(&b, DEPTH, -TERMINAL, TERMINAL);
    if (b.bestMove == -1) {
      printf("Alpha-Beta Failed! \n");
      return 1;
    }

    gprintf("Eval: %d \n", b.eval);

    int x, y;
    if (b.turn == W) {
      gprintf("White's turn! \n");
      x = b.bestMove / SIZE;
      y = b.bestMove % SIZE;
      // printf("Best move %d %d %d\n" , b.bestMove, y, x);
      // move(&b, x, y);
      // continue;
    } else {
      gprintf("Black's turn! \n");
      x = b.bestMove / SIZE;
      y = b.bestMove % SIZE;
      // printf("Best move %d %d %d\n" , b.bestMove, y, x);
      move(&b, x, y);
      continue;
    }

    gprintf("Enter x: \n");
    scanf("%d", &y);
    gprintf("Enter y: \n");
    scanf("%d", &x);
    int moveStatus = move(&b, x, y);
    if (moveStatus > 0) {
      gprintf("Invalid move, try again. \n");
    } else if (moveStatus < 0) {
      printf("Fatal error! Code: %d \n", moveStatus);
      return 1;
    }
  }
  
  printBoard(&b);

  if (b.ws > b.bs) {
    gprintf("White wins! %d-%d\n", b.ws, b.bs);
  } else if (b.ws < b.bs) {
    gprintf("Black wins! %d-%d\n", b.bs, b.ws);
  } else {
    gprintf("It's a draw! \n");
  }

  return 0;
}