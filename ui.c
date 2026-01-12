#include "ui.h"
#include <ncurses.h>

#define PLAYER_TAB_OFFSET(player) ((player) * player_tab_w)
#define MENU_OPTIONS_OFFSET(option) ((option) * menu_w)

static layout_t landscape, portrait;
static layout_t *current_layout;
static const yahtzee_t *yahtzee;

static bool is_mouse_inside_window(const WINDOW *win, int x, int y) {
  int x1, y1, x2, y2;
  getyx(win, y1, x1);
  getmaxyx(win, y2, x2);
  return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

// choose layout base on terminal size
static layout_t *choose_layout(void) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  if (cols > rows) {
    return &landscape;
  } else {
    return &portrait;
  }
}

static void change_player_tab(scores_t *s, int8_t view) {
  s->scorecard_view = view % NUM_PLAYERS;
}

static void change_player_tab_rotation(scores_t *s) {
  change_player_tab(s, s->scorecard_view + 1);
}

static const char *menu_option_name(menu_option opt) {
  static const char *names[MENU_OPTIONS_SIZE] = {"Help", "Quit"};

  if (opt < 0 || opt >= MENU_OPTIONS_SIZE)
    return "Unknown";

  return names[opt];
}

static void del_dices(field_t *f) {
  for (int i = 0; i < NUM_DICES; ++i) {
    delwin(f->dice[i]);
    f->dice[i] = NULL;
  }
}

static void del_field(field_t *f) {
  delwin(f->roll_button);
  f->roll_button = NULL;
  del_dices(f);
  delwin(f->win);
  f->win = NULL;
}

static void del_menu(menu_t *m) {
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    delwin(m->section[i]);
    m->section[i] = NULL;
  }
  delwin(m->win);
  m->win = NULL;
}

static void del_scores(scores_t *s) {
  for (int i = 0; i < NUM_PLAYERS; ++i) {
    delwin(s->player_tab[i]);
    s->player_tab[i] = NULL;
  }
  delwin(s->scorecard);
  s->scorecard = NULL;
  delwin(s->win);
  s->win = NULL;
}

static void del_layout(layout_t *l) {
  delwin(l->title);
  l->title = NULL;
  del_field(&l->field);
  del_menu(&l->menu);
  del_scores(&l->scores);
}

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

static void build_orizontal_layout(const int rows, const int cols) {
  // Title
  const int title_h = 3;
  const int title_w = cols / 2;
  const int title_x = 0;
  const int title_y = 0;
  // Field
  const int field_h = rows - title_h;
  const int field_w = title_w;
  const int field_x = title_x;
  const int field_y = title_h;
  // Roll button
  const int roll_button_h = 3;
  const int roll_button_w = 8;
  const int roll_button_x = field_x;
  const int roll_button_y = field_h - roll_button_h;
  // Scores
  const int scores_h = rows;
  const int scores_w = cols - field_w;
  const int scores_x = field_w;
  const int scores_y = 0;
  // Player tab
  const int player_tab_h = 3;
  const int player_tab_w = 16;
  const int player_tab_x = scores_x; // add PLAYER_TAB_OFSET(player) for offset
  const int player_tab_y = scores_y;
  // Menu
  const int menu_h = 3;
  const int menu_w = scores_w;
  const int menu_x = scores_x;
  const int menu_y = scores_h - menu_h;
  // Scorecard
  const int scorecard_h = scores_h - player_tab_h - menu_h;
  const int scorecard_w = scores_w;
  const int scorecard_x = scores_x;
  const int scorecard_y = player_tab_h;

  del_layout(&landscape);

  landscape.title = newwin(title_h, title_w, title_y, title_x);
  landscape.field.win = newwin(field_h, field_w, field_y, field_x);
  landscape.menu.win = newwin(menu_h, menu_w, menu_y, menu_x);
  landscape.scores.win = newwin(scores_h, scores_w, scores_y, scores_x);

  box(landscape.title, 0, 0);
  box(landscape.field.win, 0, 0);
  box(landscape.menu.win, 0, 0);
  box(landscape.scores.win, 0, 0);

  landscape.field.roll_button =
      derwin(landscape.field.win, roll_button_h, roll_button_w, roll_button_y,
             roll_button_x);

  box(landscape.field.roll_button, 0, 0);

  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    landscape.menu.section[i] =
        derwin(landscape.menu.win, menu_h, menu_w / MENU_OPTIONS_SIZE, 0,
               MENU_OPTIONS_OFFSET(i));
  }

  for (int i = 0; i < NUM_PLAYERS; ++i) {
    landscape.scores.player_tab[i] =
        derwin(landscape.scores.win, player_tab_h, player_tab_w, 0,
               PLAYER_TAB_OFFSET(i));
  }
  landscape.scores.scorecard =
      derwin(landscape.scores.win, scorecard_h, scorecard_w, player_tab_h, 0);

  wnoutrefresh(landscape.title);
  wnoutrefresh(landscape.field.win);
  wnoutrefresh(landscape.menu.win);
  wnoutrefresh(landscape.scores.win);
  doupdate();

  // Title
  wprintw(landscape.title, "Yahtzee");

  // Field
  wprintw(landscape.field.roll_button, "Roll (%d)", yahtzee->attempts);

  // Menu sections
  wprintw(landscape.menu.win, "Menu: ");
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    landscape.menu.section[i] =
        derwin(landscape.menu.win, menu_h, menu_w, menu_y + i,
               menu_x + MENU_OPTIONS_OFFSET(i));
    wprintw(landscape.menu.section[i], "%s", menu_option_name(i));
  }

  // Score tab
  for (int i = 0; i < NUM_PLAYERS; ++i) {
    landscape.scores.player_tab[i] =
        derwin(landscape.scores.win, player_tab_h, player_tab_w, player_tab_y,
               player_tab_x + PLAYER_TAB_OFFSET(i));
    wprintw(landscape.scores.player_tab[i], "Player %d", i + 1);
  }
}

// TODO!!!!!!!!
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
static void build_vertical_layout(int rows, int cols) {
  // it's not correct, only for a starting point
  // Title
  const int title_h = 3;
  const int title_w = cols / 2;
  const int title_x = 0;
  const int title_y = 0;
  // Field
  const int field_h = rows - title_h;
  const int field_w = title_w;
  const int field_x = title_x;
  const int field_y = title_h;
  // Roll button
  const int roll_button_h = 3;
  const int roll_button_w = 8;
  const int roll_button_x = field_x;
  const int roll_button_y = field_h - roll_button_h;
  // Scores
  const int scores_h = rows;
  const int scores_w = cols - field_w;
  const int scores_x = field_w;
  const int scores_y = 0;
  // Player tab
  const int player_tab_h = 3;
  const int player_tab_w = 16;
  const int player_tab_x = scores_x; // add PLAYER_TAB_OFSET(player) for offset
  const int player_tab_y = scores_y;
  // Menu
  const int menu_h = 3;
  const int menu_w = scores_w;
  const int menu_x = scores_x;
  const int menu_y = scores_h - menu_h;
  // Scorecard
  const int scorecard_h = scores_h - player_tab_h - menu_h;
  const int scorecard_w = scores_w;
  const int scorecard_x = scores_x;
  const int scorecard_y = player_tab_h;
}

void ui_init(const yahtzee_t *y) {
  current_layout = choose_layout();
  yahtzee = y;
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

  if (has_colors()) {
    start_color();
    use_default_colors();
  }

  refresh();
}

void ui_draw(void) {
  erase();   // pulisce stdscr
  refresh(); // applica

  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  build_orizontal_layout(rows, cols);
}

void ui_free(void) {
  del_layout(&landscape);
  del_layout(&portrait);

  endwin();
}
