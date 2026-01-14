#include "yahtzee.h"
#include <stdint.h>

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

void yahtzee_init(yahtzee_t *y) {
  *y = (yahtzee_t){0};

  y->attempts = 3;
  y->active_player = 0;
  y->state = PLAY;
}

/* ---------- Utils ---------- */

static void reset_dices(dice_t *dice) {
  for (int i = 0; i < NUM_DICES; ++i) {
    dice[i].selected = UNSELECTED;
  }
}

static void reset_attempts(yahtzee_t *y) { y->attempts = NUM_ATTEMPTS; }

static int sum_dices(const dice_t *d) {
  int sum = 0;
  for (int i = 0; i < NUM_DICES; ++i)
    sum += d[i].value;
  return sum;
}

// return the number of repetitions of a specific value of the dices
static int repetition(const dice_t *d, uint8_t search_val) {
  int repetition = 0;
  for (int i = 0; i < NUM_DICES; ++i)
    if (d[i].value == search_val)
      ++repetition;
  return repetition;
}

void yahtzee_roll_dices(yahtzee_t *y) {
  srand(time(NULL));
  while (y->attempts > 0) {
    for (int i = 0; i < NUM_DICES; ++i) {
      if (!y->dice[i].selected)
        y->dice[i].value = rand() % 6 + 1;
    }
    --y->attempts;
  }
}

void yahtzee_hold_dice(bool *dice_s) { *dice_s = SELECTED; }
void yahtzee_release_dice(bool *dice_s) { *dice_s = UNSELECTED; }

void yahtzee_change_turn(yahtzee_t *y) {
  y->active_player = (y->active_player + 1) % NUM_PLAYERS;
  reset_dices(y->dice);
  reset_attempts(y);
}

void yahtzee_pause_game(yahtzee_t *y) { y->state = MENU; }

/* ---------- Game logic ---------- */

// return the sum of all the dices
static int get_chance(const dice_t *d) { return sum_dices(d); }

// return the sum of all the dices
static int get_three_of_a_kind(const dice_t *d) {
  for (int i = 1; i <= NUM_FACES_DICE; ++i) {
    if (repetition(d, i) == 3) {
      // found a three of a kind
      return sum_dices(d);
      break;
    }
  }
  return 0;
}

static int get_four_of_a_kind(const dice_t *d) {
  for (int i = 1; i <= NUM_FACES_DICE; ++i) {
    if (repetition(d, i) == 4) {
      // found a four of a kind
      return sum_dices(d);
      break;
    }
  }
  return 0;
}

static int get_full_house(const dice_t *d) {
  // number of repetition of any value
  int count_rep[NUM_FACES_DICE] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  bool triple = false, couple = false;

  for (int i = 0; i < NUM_FACES_DICE; ++i) {
    if (count_rep[i] == 3) {
      triple = true;
      continue;
    }
    if (count_rep[i] == 2) {
      couple = true;
      continue;
    }
  }
  return (triple && couple) ? FULL_HOUSE_VALUE : 0;
}

static int get_small_straight(const dice_t *d) {
  int count_rep[NUM_FACES_DICE] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  bool straight_1_4 =
      count_rep[0] && count_rep[1] && count_rep[2] && count_rep[3];

  bool straight_2_5 =
      count_rep[1] && count_rep[2] && count_rep[3] && count_rep[4];
  bool straight_3_6 =
      count_rep[2] && count_rep[3] && count_rep[4] && count_rep[5];

  if (straight_1_4 || straight_2_5 || straight_3_6)
    return SMALL_STRAIGHT_VALUE;
  return 0;
}

static int get_large_straight(const dice_t *d) {
  int count_rep[NUM_FACES_DICE] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  bool straight_1_5 = count_rep[0] && count_rep[1] && count_rep[2] &&
                      count_rep[3] && count_rep[4];

  bool straight_2_6 = count_rep[1] && count_rep[2] && count_rep[3] &&
                      count_rep[4] && count_rep[5];

  if (straight_1_5 || straight_2_6)
    return LARGE_STRAIGHT_VALUE;
  return 0;
}

static int get_yahtzee(const dice_t *d) {
  for (int i = 1; i <= NUM_FACES_DICE; ++i) {
    if (repetition(d, i) == 5) {
      // found a yahtzee
      return YAHTZEE_VALUE;
      break;
    }
  }
  return 0;
}

// return true if there is at least one unselected upper combination
static bool is_there_unselected_upper_combination(const scorecard_t *card) {
  for (int i = 0; i < UPPER_SIZE; ++i) {
    if (!card->upper[i].selected) {
      return true;
    }
  }
  return false;
}

static bool is_there_unselected_lower_combination(const scorecard_t *card) {
  for (int i = 0; i < LOWER_SIZE; ++i) {
    if (!card->lower[i].selected) {
      return true;
    }
  }
  return false;
}

static bool is_joker(yahtzee_t *y) {
  return y->player[y->active_player].card.lower[YAHTZEE].points ==
             YAHTZEE_VALUE &&
         get_yahtzee(y->dice) == YAHTZEE_VALUE;
}

// return true if the an upper selection occurred
static void add_joker_bonus(yahtzee_t *y) {
  y->player[y->active_player].card.yahtzee_bonus += YAHTZEE_BONUS_VALUE;
}

selection_status yahtzee_select_upper_combination(upper_section combination,
                                                  yahtzee_t *y) {
  scorecard_t *card = &y->player[y->active_player].card;

  upper_section forced_upper = (upper_section)(y->dice[0].value - 1);
  bool is_joker_roll = is_joker(y);
  if (is_joker_roll && combination != forced_upper) {
    return UNSELECTED;
  } else if (is_joker_roll)
    add_joker_bonus(y);
  if (!card->upper[combination].selected) {
    int val = combination + 1;
    card->upper[combination].points = val * repetition(y->dice, val);
    card->upper[combination].selected = SELECTED;
    return SELECTED;
  }
  return UNSELECTED;
}

selection_status yahtzee_select_lower_combination(lower_section combination,
                                                  yahtzee_t *y) {
  scorecard_t *card = &y->player[y->active_player].card;
  uint8_t *points = &card->lower[combination].points;
  dice_t *d = y->dice;

  bool s = is_joker(y); // state of the joker
  if (s && is_there_unselected_upper_combination(card))
    return UNSELECTED;
  if (!card->lower[combination].selected) {
    switch (combination) {
    case CHANCE:
      *points = s ? sum_dices(d) : get_chance(d);
      break;
    case THREE_OF_A_KIND:
      *points = s ? sum_dices(d) : get_three_of_a_kind(d);
      break;
    case FOUR_OF_A_KIND:
      *points = s ? sum_dices(d) : get_four_of_a_kind(d);
      break;
    case FULL_HOUSE:
      *points = s ? FULL_HOUSE_VALUE : get_full_house(d);
      break;
    case SMALL_STRAIGHT:
      *points = s ? SMALL_STRAIGHT_VALUE : get_small_straight(d);
      break;
    case LARGE_STRAIGHT:
      *points = s ? LARGE_STRAIGHT_VALUE : get_large_straight(d);
      break;
    case YAHTZEE:
      *points = get_yahtzee(d);
      break;
    default:
      fprintf(stderr, "Unknown lower combination\n");
      break;
    }

    if (s)
      add_joker_bonus(y);

    card->lower[combination].selected = SELECTED;
    return SELECTED;
  }

  return UNSELECTED;
}

bool yahtzee_is_there_unselected_combination(const scorecard_t *card) {
  return is_there_unselected_upper_combination(card) ||
         is_there_unselected_lower_combination(card);
}
