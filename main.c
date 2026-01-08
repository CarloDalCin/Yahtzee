#include "yahtzee.h"
#include <ncurses.h>

int main(void) {
  yahtzee_t game;

  init_yahtzee(&game);

  game.attempts = 3;
  game.active_player = 0;

  int running = 1;
  while (running) {
    draw_yahtzee(&game);

    int c = getch();
    if (c == 'q')
      running = 0;
  }

  free_yahtzee();
  return 0;
}
