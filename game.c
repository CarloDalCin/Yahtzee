#include "game.h"
#include "ui.h"
#include "yahtzee.h"

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

void game_loop(game_t *g) {
  g->state = PLAY;

  while (g->state != STOP) {
    ui_draw(g->ui); // Disegna usando g->ui->hovered_target per gli effetti

    game_event_t ev = input_get_event(g->ui);

    switch (ev.type) {
    case EVENT_NONE:
      break;
    case EVENT_QUIT:
      change_state(g, STOP);
      break;

    case EVENT_RESIZE:
      ui_choose_appropriate_layout(g->ui);
      ui_draw(g->ui);
      break;

    case EVENT_ROLL:
      // Roll unselected dices
      yahtzee_roll_dices(g->yahtzee);
      break;

    case EVENT_CLICK:
      // Qui gestisci l'azione vera e propria in base al target
      switch (ev.target.type) {
      case ELEMENT_NONE:
        break;

      case ELEMENT_ROLL_BUTTON:
        // Roll unselected dices
        yahtzee_roll_dices(g->yahtzee);
        break;

      case ELEMENT_DICE:
        yahtzee_toggle_dice(g->yahtzee->dice, ev.target.index);
        break;

      case ELEMENT_CATEGORY: {
        selection_status s = UNSELECTED;
        if (g->ui->current_layout->scores.scorecard_view ==
            g->yahtzee->active_player) {
          switch (ev.target.index) {
          case VIEW_ACES:
          case VIEW_TWOS:
          case VIEW_THREES:
          case VIEW_FOURS:
          case VIEW_FIVES:
          case VIEW_SIXES:
            s = yahtzee_select_upper_combination(
                g->yahtzee,
                ui_combination_view_to_upper_section(ev.target.index));
          case VIEW_UPPER_BONUS:
            break;
          case VIEW_CHANCE:
          case VIEW_THREE_OF_A_KIND:
          case VIEW_FOUR_OF_A_KIND:
          case VIEW_FULL_HOUSE:
          case VIEW_SMALL_STRAIGHT:
          case VIEW_LARGE_STRAIGHT:
          case VIEW_YAHTZEE:
            s = yahtzee_select_lower_combination(
                g->yahtzee,
                ui_combination_view_to_lower_section(ev.target.index));
          case VIEW_TOTAL:
            break;
          default:
            break;
          }
          if (s == SELECTED) {
            yahtzee_change_turn(g->yahtzee);
            ui_get_next_player_tab_view(&g->ui->current_layout->scores);
          }
        }
        break;
      } // ELEMENT_CATEGORY

      case ELEMENT_MENU_OPTION:
        switch (ev.target.index) {
        case MENU_HELP:
          change_state(g, MENU);
          break;
        case MENU_QUIT:
          change_state(g, STOP);
          break;
        }
        break;

      case ELEMENT_PLAYER_TAB:
        ui_change_player_tab(&g->ui->current_layout->scores, ev.target.index);
        break;
      }
      break;

    case EVENT_HOVER:
      // update hovered element for the next frame
      g->ui->hovered_element = ev.target;
      break;
    }
  }
}
