#ifndef YAHTZEE_H
#define YAHTZEE_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define NUM_DICES 5
#define NUM_FACES_DICE 6
#define NUM_PLAYERS 2
#define NUM_ATTEMPTS 3
#define FULL_HOUSE_VALUE 25
#define SMALL_STRAIGHT_VALUE 30
#define LARGE_STRAIGHT_VALUE 40
#define YAHTZEE_VALUE 50
#define YAHTZEE_BONUS_VALUE 100

typedef enum { UNSELECTED, SELECTED } selection_status;

typedef enum { STOP, PLAY, MENU } game_state;

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
  CHANCE,
  THREE_OF_A_KIND,
  FOUR_OF_A_KIND,
  FULL_HOUSE,
  SMALL_STRAIGHT,
  LARGE_STRAIGHT,
  YAHTZEE,
  LOWER_SIZE,
} lower_section;

/* ---------- Game Struct ---------- */

typedef struct {
  struct {
    uint8_t points;
    bool selected;
  } upper[UPPER_SIZE], lower[LOWER_SIZE];
  uint8_t upper_bonus;    // 35 se >= 63
  uint16_t yahtzee_bonus; // max 1200
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
  player_t player[NUM_PLAYERS];
  dice_t dice[NUM_DICES];
  uint8_t attempts;      // attempts left on a turn
  uint8_t active_player; // 0: player 1, 1: player 2
  game_state state;
} yahtzee_t;

/* ---------- Functions ---------- */

void yahtzee_init(yahtzee_t *y);
void yahtzee_draw(const yahtzee_t *y);
void yahtzee_free(void);

/* ---------- Utils ---------- */

void yahtzee_roll_dices(yahtzee_t *y);
void yahtzee_hold_dice(bool *dice_s);
void yahtzee_release_dice(bool *dice_s);
void yahtzee_change_turn(yahtzee_t *y);
void yahtzee_pause_game(yahtzee_t *y);

// puts the value of a specific combination (obtained from the dices) in the
// scorecard
selection_status yahtzee_select_upper_combination(upper_section combination,
                                                  yahtzee_t *y);
selection_status yahtzee_select_lower_combination(lower_section combination,
                                                  yahtzee_t *y);
bool yahtzee_is_there_unselected_combination(const scorecard_t *card);

#endif // YAHTZEE_H
