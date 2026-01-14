#include "game.h"

int main(void) {
  game_t *game = game_init();

  while (game->state != STOP) {

    game->input = getch();
    if (game->input == 'q')
      game->state = STOP;
    else if (game->input == KEY_RESIZE)
      choose_appropriate_layout(game->ui);
  }

  game_free(&game);
  return 0;
}
