#include "ui.h"
#include "yahtzee.h"
#include <ncurses.h>

int main(void) {
  yahtzee_t *game = ui_init();

  while (game->state != STOP) {
    ui_draw();

    int c = getch();
    if (c == 'q')
      game->state = STOP;
    else if (c == KEY_RESIZE)
      choose_appropriate_layout();
  }

  ui_free();

  return 0;
}
