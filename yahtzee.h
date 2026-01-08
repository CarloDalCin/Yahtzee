#ifndef YAHTZEE_H
#define YAHTZEE_H

#include <stdbool.h>
#include <stdint.h>

/* ---------- Point Section ---------- */

enum upper_section { ACES, TWOS, THREES, FOURS, FIVES, SIXES };

enum lower_section {
  THREE_OF_A_KIND,
  FOUR_OF_A_KIND,
  FULL_HOUSE,
  SMALL_STRAIGHT,
  LARGE_STRAIGHT,
  CHANCE,
  YAHTZEE
};

/* ---------- Game Struct ---------- */

typedef struct {
  uint16_t yahtzee_bonus; // max 1200
  uint8_t upper[6];
  uint8_t upper_bonus; // 35 se >= 63
  uint8_t lower[7];
} scorecard_t;

typedef struct {
  scorecard_t card;
  uint16_t total_score;
  uint8_t upper_section_score;
} player_t;

typedef struct {
  player_t player[2];
  uint8_t attempts;   // attempts left on a turn
  bool active_player; // 0: player 1, 1: player 2
  uint8_t runnig;     // 0: not running, 1: running
} yahtzee_t;

/* ---------- Functions ---------- */

void init_yahtzee(yahtzee_t *y);
void draw_menu(const yahtzee_t *y);
void draw_yahtzee(const yahtzee_t *y);
void free_yahtzee();

#endif // YAHTZEE_H
