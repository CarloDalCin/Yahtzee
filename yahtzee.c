#include "yahtzee.h"
#include <stdint.h>

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

void init_yahtzee(yahtzee_t *y) {
  *y = (yahtzee_t){0};

  /* init ncurses */
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0); // nasconde il cursore
}

void draw_menu(const yahtzee_t *y) {}

void draw_yahtzee(const yahtzee_t *y) {
  clear();

  /* Titolo */
  mvprintw(1, 10, "YAHTZEE");

  /* Info giocatore */
  int p = y->active_player;
  mvprintw(3, 2, "Player: %d", p + 1);
  mvprintw(4, 2, "Attempts: %d", y->attempts);

  /* Upper section */
  mvprintw(6, 2, "Upper section:");
  for (int i = 0; i < 6; i++) {
    mvprintw(7 + i, 4, "%d: %d", i + 1, y->player[p].card.upper[i]);
  }

  /* Lower section */
  mvprintw(14, 2, "Lower section:");
  for (int i = 0; i < 7; i++) {
    mvprintw(15 + i, 4, "%d: %d", i + 1, y->player[p].card.lower[i]);
  }

  refresh();
}

void free_yahtzee(void) {
  endwin(); // ripristina il terminale
}

/* ---------- Utils ---------- */

void launch_dices(yahtzee_t *y) {
  srand(time(NULL));
  if (y->attempts > 0) {
    for (int i = 0; i <= NUM_DICES; ++i) {
      if (!y->dice[i].selected)
        y->dice[i].value = rand() % 6 + 1;
    }
    --y->attempts;
  }
}

static void reset_dices(yahtzee_t *y) {
  for (int i = 0; i <= NUM_DICES; ++i) {
    y->dice[i].selected = false;
  }
}

// reset attempts and unselect dices
void reset_attempts(yahtzee_t *y) {
  reset_dices(y);
  y->attempts = 3;
}

void toggle_dice(dice_t *d) { d->selected = !d->selected; }

void change_turn(yahtzee_t *y) { y->active_player = !y->active_player; }

void pause_game(yahtzee_t *y) { y->runnig = 0; }

int sum_dices(const dice_t *d) {
  int sum = 0;
  for (int i = 0; i <= NUM_DICES; ++i)
    sum += d[i].value;
  return sum;
}

// return the number of repetitions of a specific value of the dices
int repetition(const dice_t *d, uint8_t search_val) {
  int repetition = 0;
  for (int i = 0; i <= NUM_DICES; ++i)
    if (d[i].value == search_val)
      ++repetition;
  return repetition;
}

/* ---------- Game logic ---------- */

void select_upper_combination(upper_section combination, yahtzee_t *y) {
  uint8_t *comb = &y->player[y->active_player].card.upper[combination];
  if (!*comb) {
    *comb = sum_dices(y->dice);
  }
}

// return the sum of all the dices
static int get_chance(dice_t *d) { return sum_dices(d); }

// return the sum of all the dices
static int get_three_of_a_kind(dice_t *d) {
  for (int i = 1; i <= 6; ++i) {
    if (repetition(d, i) == 3) {
      // found a three of a kind
      return sum_dices(d);
      break;
    }
  }
  return 0;
}

static int get_four_of_a_kind(dice_t *d) {
  for (int i = 1; i <= 6; ++i) {
    if (repetition(d, i) == 4) {
      // found a four of a kind
      return sum_dices(d);
      break;
    }
  }
  return 0;
}

static int get_full_house(dice_t *d) {
  // number of repetition of any value
  int count_rep[6] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  bool triple = false, couple = false;

  for (int i = 0; i < 6; ++i) {
    if (count_rep[i] == 3) {
      triple = true;
      break;
    }
    if (count_rep[i] == 2) {
      couple = true;
      break;
    }
  }
  return (triple && couple) ? 25 : 0;
}

static int get_small_straight(dice_t *d) {
  int count_rep[6] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  bool small_straight = true;
  for (int i = 0; i < 6; ++i) {
    if (count_rep[i] > 2) {
      small_straight = false;
      break;
    }
  }
  return small_straight ? 30 : 0;
}

static int get_large_straight(dice_t *d) {
  int count_rep[6] = {
      [0] = repetition(d, 1), [1] = repetition(d, 2), [2] = repetition(d, 3),
      [3] = repetition(d, 4), [4] = repetition(d, 5), [5] = repetition(d, 6),
  };
  return (count_rep[0] && count_rep[1] && count_rep[2] && count_rep[3] &&
          count_rep[4] && count_rep[5])
             ? 40
             : 0;
}

void select_lower_combination(lower_section combination, yahtzee_t *y) {}
