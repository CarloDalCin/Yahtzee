#include "yahtzee.h"
#include <ncurses.h>

void init_yahtzee(yahtzee_t *y) {
  *y = (yahtzee_t){0};

  /* init ncurses */
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0); // nasconde il cursore
}

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
