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

static upper_section view_to_upper(int view_idx) {
  // In ui.h: VIEW_ACES = 0 ...
  return (upper_section)view_idx;
}

static lower_section view_to_lower(int view_idx) {
  // view_idx parte da VIEW_ACES (0). VIEW_CHANCE è 7 (secondo la enum in ui.h)
  // lower_section enum parte da CHANCE = 0.
  return (lower_section)(view_idx - VIEW_CHANCE);
}

static bool is_upper_view(int view_idx) {
  return view_idx >= VIEW_ACES && view_idx <= VIEW_SIXES;
}

static bool is_lower_view(int view_idx) {
  return view_idx >= VIEW_CHANCE && view_idx <= VIEW_YAHTZEE;
}

void game_loop(game_t *g) {
  g->state = PLAY;
  game_event_t ev;

  while (g->state != STOP) {
    // 1. Draw
    ui_draw(g->ui);

    // 2. Input
    ev = input_get_event(g->ui);

    // 3. Logic Update
    switch (ev.type) {
    case EVENT_QUIT:
      change_state(g, STOP);
      break;

    case EVENT_RESIZE:
      choose_appropriate_layout(g->ui);
      break;

    case EVENT_ROLL:
      // Logica: Rollare solo se abbiamo tentativi e non è fine partita
      if (g->yahtzee->attempts > 0 && !yahtzee_end_game(g->yahtzee)) {
        yahtzee_roll_dices(g->yahtzee);
      }
      break;

    case EVENT_TOGGLE_DICE:
      // Toggle selezione dado (solo se non siamo al primo roll)
      if (g->yahtzee->attempts < NUM_ATTEMPTS) {
        dice_t *d = &g->yahtzee->dice[ev.index];
        d->selected = !d->selected;
      }
      break;

    case EVENT_SELECT_CATEGORY:
      // Logica complessa: validare la selezione
      // L'utente può selezionare solo se ha tirato i dadi almeno una volta
      if (g->yahtzee->attempts < NUM_ATTEMPTS) {
        selection_status status = UNSELECTED;

        if (is_upper_view(ev.index)) {
          status = yahtzee_select_upper_combination(view_to_upper(ev.index),
                                                    g->yahtzee);
        } else if (is_lower_view(ev.index)) {
          status = yahtzee_select_lower_combination(view_to_lower(ev.index),
                                                    g->yahtzee);
        }

        if (status == SELECTED) {
          // Turno finito, passa al prossimo giocatore
          yahtzee_change_turn(g->yahtzee);
          // Resetta UI per mostrare la card del nuovo giocatore attivo
          g->ui->current_layout->scores.scorecard_view =
              g->yahtzee->active_player;
        }
      }
      break;

    case EVENT_SWITCH_TAB:
      // Visualizza scorecard di un altro giocatore (sola lettura)
      g->ui->current_layout->scores.scorecard_view = ev.index;
      break;

    default:
      break;
    }
  }
}
