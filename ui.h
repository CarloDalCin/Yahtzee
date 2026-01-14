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
  int8_t scorecard_view; // view scorecard of 0: payer 1, 1: player 2 ...
} scores_t;

typedef struct {
  WINDOW *title;
  field_t field;
  menu_t menu;
  scores_t scores;
} layout_t;

yahtzee_t *ui_init(void);
void choose_appropriate_layout(void);
void ui_draw(void);
void ui_free(void);

#endif // UI_H
