#include "ui.h"
#include "yahtzee.h"
#include <ncurses.h>

int main(void) {
  yahtzee_t game;

  yahtzee_init(&game);

  game.attempts = 3;
  game.active_player = 0;

  ui_init(&game);
  while (game.state != STOP) {
    ui_draw();

    int c = getch();
    if (c == 'q')
      game.state = STOP;
  }

  yahtzee_free();
  return 0;
}
