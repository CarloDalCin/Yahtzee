// yahtzee lib that contain the logic of the game
#ifndef YAHTZEE_H
#define YAHTZEE_H

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
#define UPPER_BONUS_TRESHHOLD 63
#define UPPER_BONUS_VALUE 35

typedef enum { UNSELECTED, SELECTED } selection_status;

typedef enum {
  END_GAME = false,
  NEXT_TURN = true,
} turn_status;

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
} yahtzee_t;

/* ---------- Functions ---------- */

yahtzee_t *yahtzee_init(void);
void yahtzee_free(yahtzee_t **y);

/* ---------- Utils ---------- */

void yahtzee_roll_dices(yahtzee_t *y);
void yahtzee_hold_dice(bool *dice_s);
void yahtzee_release_dice(bool *dice_s);
void yahtzee_toggle_dice(dice_t *dice, int index);
turn_status yahtzee_change_turn(yahtzee_t *y);
void yahtzee_end_game(yahtzee_t *y);

// puts the value of a specific combination (obtained from the dices) in the
// scorecard
selection_status yahtzee_select_upper_combination(yahtzee_t *y,
                                                  upper_section combination);
selection_status yahtzee_select_lower_combination(yahtzee_t *y,
                                                  lower_section combination);
bool yahtzee_is_there_unselected_upper_combination(const scorecard_t *card);
bool yahtzee_is_there_unselected_lower_combination(const scorecard_t *card);
bool yahtzee_is_there_unselected_combination(const scorecard_t *card);

#endif // YAHTZEE_H
