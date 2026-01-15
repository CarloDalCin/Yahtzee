#include "game.h"

int main(void) {
  game_t *game = game_init();

  game_loop(game);

  game_free(&game);
  return 0;
}
