#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

uint32_t x=123456789;
uint32_t y=362436069;
uint32_t z=521288629;
uint32_t w=88675123; 
uint32_t depth = 10;
uint32_t sampling = 500;

uint32_t xorshift128(void) {
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
}

void print(uint32_t board[4][4]) {
  printf("%5d %5d %5d %5d\n", board[0][0], board[0][1], board[0][2], board[0][3]);
  printf("%5d %5d %5d %5d\n", board[1][0], board[1][1], board[1][2], board[1][3]);
  printf("%5d %5d %5d %5d\n", board[2][0], board[2][1], board[2][2], board[2][3]);
  printf("%5d %5d %5d %5d\n", board[3][0], board[3][1], board[3][2], board[3][3]);
  printf("\n");
}

bool cleared(uint32_t board[4][4]){
  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<4; j++){
      if (board[i][j] == 2048) return true;
    }
  }
  return false;
}

uint32_t countZero(uint32_t board[4][4]){
  uint32_t ret = 0;
  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<4; j++){
      if (board[i][j] == 0) ++ret;
    }
  }
  return ret;
}

void transposition(uint32_t board[4][4]){
  uint32_t tmp;
  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<4; j++){
      if(i >= j || board[i][j] == board[j][i]) continue;
      tmp = board[i][j];
      board[i][j] = board[j][i];
      board[j][i] = tmp;
    }
  }
}

void innerreverse(uint32_t board[4][4]){
  uint32_t tmp;
  for (uint32_t i=0; i<4; i++){
    if (board[i][0] != board[i][3]){
      tmp = board[i][0];
      board[i][0] = board[i][3];
      board[i][3] = tmp;
    }
    if (board[i][1] != board[i][2]){
      tmp = board[i][1];
      board[i][1] = board[i][2];
      board[i][2] = tmp;
    }
  }
}

void add(uint32_t board[4][4]) {
  uint32_t z = countZero(board);
  if (z == 0) return;
  uint32_t n = xorshift128() % z;
  uint32_t v = xorshift128() % 10 == 0 ? 4 : 2;

  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<4; j++){
      if (board[i][j] == 0 && n == 0){
        board[i][j] = v;
        return;
      }
      if (board[i][j] == 0) --n;
    }
  }
}

void preprocess(uint32_t board[4][4], uint32_t direction){
  switch (direction) {
    case 1:
      innerreverse(board);
      break;
    case 2:
      transposition(board);
      break;
    case 3:
      transposition(board);
      innerreverse(board);
      break;
  }
}

void postprocess(uint32_t board[4][4], uint32_t direction){
  switch (direction) {
    case 1:
      innerreverse(board);
      break;
    case 2:
      transposition(board);
      break;
    case 3:
      innerreverse(board);
      transposition(board);
      break;
  }
}

bool merge(uint32_t board[4][4]) {
  bool ret = false;
  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<3; j++){
      if (board[i][j] == 0){
        for (uint32_t k=j+1; k<4; k++){
          if (board[i][k] != 0){
            board[i][j] = board[i][k];
            board[i][k] = 0;
            ret = true;
            break;
          }
        }
      }
    }
  }

  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<3; j++){
      if (board[i][j] > 0 && board[i][j] == board[i][j+1]){
        board[i][j] = board[i][j] * 2;
        board[i][j+1] = 0;
        ret = true;
      }
    }
  }

  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<3; j++){
      if (board[i][j] == 0){
        for (uint32_t k=j+1; k<4; k++){
          if (board[i][k] != 0){
            board[i][j] = board[i][k];
            board[i][k] = 0;
            ret = true;
            break;
          }
        }
      }
    }
  }

  return ret;
}

bool mergeable(uint32_t board[4][4]) {
  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<3; j++){
      if (board[i][j] == 0){
        for (uint32_t k=j+1; k<4; k++){
          if (board[i][k] != 0) return true;
        }
      }
    }
  }

  for (uint32_t i=0; i<4; i++){
    for (uint32_t j=0; j<3; j++){
      if (board[i][j] > 0 && board[i][j] == board[i][j+1]) return true;
    }
  }
  return false;
}

bool movable(uint32_t board[4][4], uint32_t direction) {
  uint32_t b[4][4];
  memcpy(b, board, sizeof(uint32_t) * 16);
  preprocess(b, direction);
  return mergeable(b);
}

bool move(uint32_t board[4][4], uint32_t direction) {
  bool ret;
  preprocess(board, direction);
  ret = merge(board);
  postprocess(board, direction);
  return ret;
}

bool randommove(uint32_t board[4][4]) {
  uint32_t directions[4];
  uint32_t n = 0;
  for (int i = 0; i < 4; i++) {
    if(movable(board, i)) {
      directions[n] = i;
      ++n;
    }
  }
  if (n == 0) return false;
  return move(board, directions[xorshift128() % n]);
}

int32_t evaluate(uint32_t board[4][4], uint32_t direction){
  uint32_t directionboard[4][4];
  uint32_t samplingboard[4][4];
  uint32_t zerocells = 0;
  uint32_t count = 0;

  memcpy(directionboard, board, sizeof(uint32_t) * 16);

  if (!move(directionboard, direction)) return -1;
  zerocells = countZero(directionboard);
  add(directionboard);

  for (uint32_t i = 0; i < sampling; ++i) {
    memcpy(samplingboard, directionboard, sizeof(uint32_t) * 16);
    for (uint32_t j = 0; j < depth; ++j) {
      /* ~80 / 100 */
      if (randommove(samplingboard)) {
        add(samplingboard);
      } else {
        break;
      }

      /* ~60 / 100 */
      /* if (move(samplingboard, xorshift128() % 4)) { */
      /*   add(samplingboard); */
      /* } else { */
      /*   if (countZero(samplingboard) == 0) break; */
      /* } */

      /* ~60 / 100 */
      /* move(samplingboard, xorshift128() % 4); */
      /* if (countZero(samplingboard) == 0) break; */
      /* add(samplingboard); */

      /* 44 / 100 */
      /* move(samplingboard, xorshift128() % 4); */
      /* add(samplingboard); */
      /* if (countZero(samplingboard) == 0) break; */

    }
    count += countZero(samplingboard);
  }

  return count;
}

int32_t predict(uint32_t board[4][4]) {
  int32_t v = -100;
  int32_t tmp;
  int32_t direction = 0;
  for (int i = 0; i < 4; i++) {
    tmp = evaluate(board, i);
    if (tmp > v) {
      v = tmp;
      direction = i;
    }
  }
  return direction;
}

int main(int argc, char const* argv[])
{
  x = clock();

  if (argc == 3) {
    sscanf (argv[1],"%d",&depth);
    sscanf (argv[2],"%d",&sampling);
  } else {
    depth = 15;
    sampling = 500;
  }

  uint32_t board[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  while(true){
    add(board);
    move(board, predict(board));
    if (cleared(board)) break;
    if (countZero(board) == 0) break;
  }
  if (countZero(board) == 0) return 1;

  return 0;
}
