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
    score += colMul * 10;
  } else {
    score += colMul * -5;
  }
}

int getBoardScore(Board *b) {
  // in case game is done
  if (b->ws + b->bs == SIZE * SIZE) {
    if (b->ws > b->bs) return TERMINAL;
    else if (b->bs > b->ws) return -TERMINAL;
    else return 0;
  }

  int score = 0;
  for (int i = 0; i < SIZE; ++i) {
    for (int j = 0; j < SIZE; ++j) {
      score += getSquareScore(b, i, j);
    }
  }

  // check ratio of (ws to bs) to game time
  int wbDiff = b->ws - b->bs;
  int time = b->ws + b->bs;
  int maxTime = SIZE * SIZE;
  score += ((time - 3 * maxTime / 4) * wbDiff) / (maxTime /  2);

  // account for number of moves
  int numMoves = getNumMoves(b);
  if (b->turn == W) {
    score += (numMoves - AVG_MOVES) * 8;
  } else {
    score -= (numMoves - AVG_MOVES) * 8;
  }

  return score;
}

void sortMoves(Move *moves) {
  // partition
  int idx = 0;
  for (int i = 0; i < SIZE * SIZE; ++i) {
    if (moves[i].coord != -1) {
      Move temp = moves[i];
      moves[i] = moves[idx];
      moves[idx] = temp;
      ++idx;
    }
  }

  // sort
  int maxIdx;
  for (int i = 0; i < idx; ++i) {
    maxIdx = i;
    for (int j = i + 1; j < idx; ++j) {
      if (moves[j].coord == -1) continue; // skip the shit ones

      int x = moves[j].coord / SIZE, y = moves[j].coord % SIZE;
      if (  // corner always best
        (x == 0 && y == 0) ||
        (x == 0 && y == SIZE - 1) ||
        (x == SIZE - 1 && y == 0) ||
        (x == SIZE - 1 && y == SIZE - 1)
      ) {
        maxIdx = j;
        break;
      }

      if (
        (moves[j].numMoves + moves[j].numFlipped) >
        (moves[maxIdx].numMoves > moves[maxIdx].numFlipped)
      ) {
        maxIdx = j;
      }
    }
    Move temp = moves[i];
    moves[i] = moves[maxIdx];
    moves[maxIdx] = temp;
  }
}

int alphaBetaProb(Board *board, int depth, int a, int b);

int alphaBeta(Board *board, int depth, int a, int b) {
  srand(time(NULL));

  int eval, bestMove = -1;
  if (board->turn == W) {
    eval = -TERMINAL;
  } else {
    eval = TERMINAL;
  }
  Board scrapB;
  for (int i = 0; i < NUM_RUNS; ++i) {
    copyBoard(board, &scrapB);
    int iterEval = alphaBetaProb(&scrapB, depth, a, b);
    if (board->turn == W) {
      if (iterEval >= eval) {
        eval = iterEval;
        bestMove = scrapB.bestMove;
      }
    } else {
      if (iterEval <= eval) {
        eval = iterEval;
        bestMove = scrapB.bestMove;
      }
    }
  }
  board->eval = eval;
  board->bestMove = bestMove;
  return eval;
}

int alphaBetaProb(Board *board, int depth, int a, int b) {
  if (depth == 0) return getBoardScore(board);

  Board scrapB;
  Square turn = board->turn;
  board->bestMove = -1;
  board->eval = 0;
  int value;
  int hasMoved = 0;
  Move moves[SIZE * SIZE];
  getMoves(board, moves);
  sortMoves(moves);
  if (turn == W) {
    value = -TERMINAL;
    for (int i = 0; i < SIZE * SIZE; ++i) {
      int numFlipped = moves[i].numFlipped;
      int coord = moves[i].coord;
      // printf("Num flipped %d, x: %d, y: %d \n", numFlipped, i / SIZE, i % SIZE);
      if (coord == -1) break;
      if (hasMoved && rand() > RAND_MAX / ODDS) continue;
      hasMoved = 1;
      copyBoard(board, &scrapB);
      move(&scrapB, coord / SIZE, coord % SIZE);
      // printf("Move and status %d, %d | %d\n", i, j, moveStatus);
      int moveEval = alphaBetaProb(&scrapB, depth - 1, a, b);
      if (moveEval >= value) {
        // printf("Move and status %d, %d\n", i, j);
        value = moveEval;
        board->bestMove = coord;
        board->eval = value;
      }
      if (value >= b) {
        break;
      }
      if (value > a) a = value;
    }
    return value;
  } else {
    value = TERMINAL;
    for (int i = 0; i < SIZE * SIZE; ++i) {
      copyBoard(board, &scrapB);
      int numFlipped = moves[i].numFlipped;
      int coord = moves[i].coord;
      if (coord == -1) break;
      if (hasMoved && rand() > RAND_MAX / ODDS) continue;
      hasMoved = 1;
      copyBoard(board, &scrapB);
      move(&scrapB, coord / SIZE, coord % SIZE);
      int moveEval = alphaBetaProb(&scrapB, depth - 1, a, b);
      if (moveEval <= value) {
        value = moveEval;
        board->bestMove = coord;
        board->eval = value;
      }
      if (value <= a) {
        break;
      }
      if (value > b) b = value;
    }
    return value;
  }
}

// int calcEval(PTree *pt, int depth) {
//   if (depth < 0) return -1;
  
//   if (depth == 0) {
//     pt->eval = getBoardScore(pt->board);
//     return 0;
//   }

//   Square turn = pt->board->turn;
//   int bestScore = 0;
//   if (turn == W) bestScore = -100000;
//   if (turn == B) bestScore = 100000;
//   int bestMove = -1;
//   for (int i = 0; i < SIZE; ++i) {
//     for (int j = 0; j < SIZE; ++j) {
//       int moveStatus = move(&pt, i, j);
//       if (moveStatus == 0) {
//         calcEval(pt, depth - 1);
//         if (turn == W) {
//           if (pt->eval > bestScore) {
//             bestMove = toCoord(i, j);
//             bestScore = pt->eval;
//           }
//         } else {
//           if (pt->eval < bestScore) {
//             bestMove = toCoord(i, j);
//             bestScore = pt->eval;
//           }
//         }
//         pt = pt->prev;
//         freePTree(pt->childs[--pt->numChilds]);
//         pt->childs[pt->numChilds] = NULL;
//       }
//     }
//   }

//   pt->eval = bestScore;
//   pt->bestMove = bestMove;

//   return 0;
// }