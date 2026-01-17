#ifndef UI_H
#define UI_H

#include "yahtzee.h"
#include <ncurses.h>
#include <panel.h>

// orizontal layout
/*************** LAYOUT ***************
┌───────────────────────────┬──────────────────────────────┐
│        TITLE              │   SCORECARD (Player tab)     │
├───────────────────────────┼──────────────────────────────┤
│       (FIELD)             │                              │
│        DADI               │   SCORECARD (Points)         │
│                           │                              │
│                           │                              │
├─────────────────┐         ├──────────────────────────────┤
│   ROLL BUTTON   │         │          MENU                │
└─────────────────┴─────────┴──────────────────────────────┘
**************************************/

// vertical layout
/*************** LAYOUT ***************
┌────────────────────────────┐
│        TITLE               │
├────────────────────────────┤
│          MENU              │
├────────────────────────────┤
│       (FIELD)              │
│        DADI                │
│                            │
│                            │
├────────────────────────────┤
│   SCORECARD (Player tab)   │
├────────────────────────────┤
│                            │
│   SCORECARD (Points)       │
│                            │
│                            │
├────────────────────────────┤
│   ROLL BUTTON              │
└────────────────────────────┘
**************************************/

typedef enum { MENU_HELP, MENU_QUIT, MENU_OPTIONS_SIZE } menu_option;

typedef enum { LANDSCAPE, PORTRAIT, LAYOUT_SIZE } layout;

typedef enum {
  VIEW_ACES,
  VIEW_TWOS,
  VIEW_THREES,
  VIEW_FOURS,
  VIEW_FIVES,
  VIEW_SIXES,
  VIEW_UPPER_BONUS,
  VIEW_CHANCE,
  VIEW_THREE_OF_A_KIND,
  VIEW_FOUR_OF_A_KIND,
  VIEW_FULL_HOUSE,
  VIEW_SMALL_STRAIGHT,
  VIEW_LARGE_STRAIGHT,
  VIEW_YAHTZEE,
  VIEW_TOTAL,
  VIEW_COMBINATION_SIZE
} combination_view;

typedef enum {
  ELEMENT_NONE,
  ELEMENT_ROLL_BUTTON,
  ELEMENT_DICE,        // index
  ELEMENT_CATEGORY,    // index of the combination_view
  ELEMENT_MENU_OPTION, // index
  ELEMENT_PLAYER_TAB,  // index
} ui_element;

typedef struct {
  ui_element type;
  int index; // index of the targeted element. es (0-4) for dice
} ui_target_t;

typedef struct {
  WINDOW *win;
  WINDOW *dice[NUM_DICES];
  WINDOW *roll_button;
} field_t;

typedef struct {
  WINDOW *win;
  WINDOW *section[MENU_OPTIONS_SIZE];
} menu_t;

typedef struct {
  WINDOW *win;
  WINDOW *player_tab[NUM_PLAYERS];
  WINDOW *scorecard;
  WINDOW *combination_view[VIEW_COMBINATION_SIZE];
  int8_t scorecard_view; // view scorecard of 0: payer 1, 1: player 2 ...
} scores_t;

typedef struct {
  WINDOW *title;
  field_t field;
  menu_t menu;
  scores_t scores;
} layout_t;

typedef struct {
  layout_t layout[LAYOUT_SIZE];
  layout_t *current_layout;
  yahtzee_t *yahtzee;

  ui_target_t hovered_element;
} ui_t;

ui_t *ui_init(yahtzee_t *y);
void ui_choose_appropriate_layout(ui_t *ui);
void ui_draw(ui_t *ui);
void ui_free(ui_t **ui);
bool ui_is_mouse_inside_window(const WINDOW *win, int x, int y);
ui_target_t ui_pick_target(ui_t *ui, int x, int y);
void ui_change_player_tab(scores_t *s, int8_t view);
int ui_get_next_player_tab_view(scores_t *s);
int ui_get_previous_player_tab_view(scores_t *s);
upper_section ui_combination_view_to_upper_section(combination_view view);
lower_section ui_combination_view_to_lower_section(combination_view view);
int ui_combination_view_to_section(combination_view view);

#endif // UI_H
