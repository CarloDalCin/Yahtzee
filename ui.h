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

typedef enum { HELP, QUIT, MENU_OPTIONS_SIZE } menu_option;

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
  TARGET_NONE,
  TARGET_ROLL_BUTTON,
  TARGET_DICE,
  TARGET_CATEGORY,
  TARGET_MENU_OPTION,
  TARGET_PLAYER_TAB
} click_target_type;

typedef struct {
  click_target_type type;
  int index; // Indice del dado (0-4) o della categoria
} click_target_t;

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
} ui_t;

ui_t *ui_init(yahtzee_t *y);
void choose_appropriate_layout(ui_t *ui);
void ui_draw(ui_t *ui);
void ui_free(ui_t **ui);
click_target_t ui_get_click_target(ui_t *ui, int x, int y);

#endif // UI_H
