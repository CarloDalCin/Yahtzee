#include "game.h"

static void change_state(game_t *game, game_state state) {
  game->state = state;
}

game_t *game_init(void) {
  game_t *game = malloc(sizeof(game_t));

  game->yahtzee = yahtzee_init();
  game->ui = ui_init(game->yahtzee);
  game->input = 0;

  return game;
}

void game_free(game_t **game) {
  ui_free(&(*game)->ui);
  yahtzee_free(&(*game)->yahtzee);
  free(*game);
  *game = NULL;
}

void game_loop(game_t *game) {
  game->state = PLAY;

  while (game->state != STOP) {
    input_handle(&game->input);
    switch (game->state) {
    case PLAY:
      break;

    case MENU:
      break;
    default:
      break;
    }
  }
}
