#ifndef GAME_H
#define GAME_H

#include "input.h"
#include "ui.h"
#include "yahtzee.h"

typedef enum { STOP, PLAY, MENU } game_state;

typedef struct {
  yahtzee_t *yahtzee;
  ui_t *ui;
  game_state state;
  int input;
} game_t;

// return a game_t struct allocated on the heap
game_t *game_init(void);

// free the game_t struct
void game_free(game_t **game);

void game_loop(game_t *game);

#endif // GAME_H
