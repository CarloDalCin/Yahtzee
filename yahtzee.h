#ifndef YAHTZEE_H
#define YAHTZEE_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define NUM_DICES 5

/* ---------- Point Section ---------- */

typedef enum {
  ACES,
  TWOS,
  THREES,
  FOURS,
  FIVES,
  SIXES,
  UPPER_SIZE
} upper_section;

typedef enum {
  THREE_OF_A_KIND,
  FOUR_OF_A_KIND,
  FULL_HOUSE,
  SMALL_STRAIGHT,
  LARGE_STRAIGHT,
  CHANCE,
  YAHTZEE,
  LOWER_SIZE
} lower_section;

/* ---------- Game Struct ---------- */

typedef struct {
  uint16_t yahtzee_bonus; // max 1200
  uint8_t upper[UPPER_SIZE];
  uint8_t upper_bonus; // 35 se >= 63
  uint8_t lower[LOWER_SIZE];
} scorecard_t;

typedef struct {
  scorecard_t card;
  uint16_t total_score;
  uint8_t upper_section_score;
} player_t;

typedef struct {
  uint8_t value; // 1-6
  // only unselected dices are relaunchable
  bool selected; // false: unselected, true: selected
} dice_t;

typedef struct {
  player_t player[2];
  dice_t dice[NUM_DICES];
  uint8_t attempts;   // attempts left on a turn
  bool active_player; // 0: player 1, 1: player 2
  uint8_t runnig;     // 0: not running, 1: running
} yahtzee_t;

/* ---------- Functions ---------- */

void init_yahtzee(yahtzee_t *y);
void draw_menu(const yahtzee_t *y);
void draw_yahtzee(const yahtzee_t *y);
void free_yahtzee(void);

/* ---------- Utils ---------- */

void launch_dices(yahtzee_t *y);
void reset_attempts(yahtzee_t *y);
void toggle_dice(dice_t *d);
void change_turn(yahtzee_t *y);
void pause_game(yahtzee_t *y);

// puts the value of a specific combination (obtained from the dices) in the
// scorecard
void select_upper_combination(upper_section combination, yahtzee_t *y);
void select_lower_combination(lower_section combination, yahtzee_t *y);

#endif // YAHTZEE_H
