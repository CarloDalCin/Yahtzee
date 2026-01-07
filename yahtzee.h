#ifndef YAHTZEE_H
#define YAHTZEE_H

#include "include/tuibox.h"

enum upper_section {
    ACES,
    TWOS,
    THREES,
    FOURS,
    FIVES,
    SIXES
};

enum lower_section {
    THREE_OF_A_KIND,
    FOUR_OF_A_KIND,
    FULL_HOUSE,
    SMALL_STRAIGHT,
    LARGE_STRAIGHT,
    CHANCE,
    YAHTZEE
};

// byte: 16
typedef struct scorecard_t {
    uint16_t yahtzee_bonus = 0; // max 1,200
    uint8_t upper[6] = {0};
    uint8_t upper_bonus = 0; // 35 if sum(upper[0..5] >= 63)
    uint8_t lower[7] = {0};
} scorecard_t;

typedef struct player_t {
    scorecard_t card;
    uint16_t total_score = 0;
    uint8_t upper_section_score = 0;
} player_t;

typedef struct yahtzee_t {
    player_t player[2];
    uint8_t dices[5] = {0};
    bool active_player = 0; // 0: player 1, 1: player 2
} yahtzee_t;


#endif // YAHTZEE_H
