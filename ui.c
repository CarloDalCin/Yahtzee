#include "ui.h"
#include "yahtzee.h"

#define PLAYER_TAB_WIDTH 14
#define PLAYER_TAB_OFFSET(player) ((player) * PLAYER_TAB_WIDTH)
#define MENU_OPTION_WIDTH 8
#define MENU_OPTIONS_OFFSET(option) ((option) * MENU_OPTION_WIDTH)

// choose layout base on terminal size
void choose_appropriate_layout(ui_t *ui) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  if ((cols / 2) > rows) {
    ui->current_layout = &ui->layout[LANDSCAPE];
  } else {
    ui->current_layout = &ui->layout[PORTRAIT];
  }
}

static bool is_layout_active(ui_t *ui, layout l) {
  return ui->current_layout == &ui->layout[l];
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

static const char *combination_view_name(combination_view view) {
  static const char *names[VIEW_COMBINATION_SIZE] = {
      "Aces",           "Twos",       "Threes",
      "Fours",          "Fives",      "Sixes",
      "Upper Bonus",    "Chance",     "Three of a Kind",
      "Four of a Kind", "Full House", "Small Straight",
      "Large Straight", "Yahtzee",    "Total"};

  if (view < 0 || view >= VIEW_COMBINATION_SIZE)
    return "Unknown";

  return names[view];
}

static void del_win(WINDOW **w) {
  if (*w != NULL) {
    delwin(*w);
    *w = NULL;
  }
}

static void del_dices(field_t *f) {
  for (int i = 0; i < NUM_DICES; ++i)
    del_win(f->dice + i);
}

static void del_field(field_t *f) {
  del_win(&f->roll_button);
  del_dices(f);
  del_win(&f->win);
}

static void del_menu(menu_t *m) {
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i)
    del_win(m->section + i);
  del_win(&m->win);
}

static void del_scores(scores_t *s) {
  for (int i = 0; i < NUM_PLAYERS; ++i)
    del_win(s->player_tab + i);
  for (int i = 0; i < VIEW_COMBINATION_SIZE; ++i)
    del_win(s->combination_view + i);
  del_win(&s->scorecard);
  del_win(&s->win);
}

static void del_layout(layout_t *l) {
  del_win(&l->title);
  del_field(&l->field);
  del_menu(&l->menu);
  del_scores(&l->scores);
}

bool is_mouse_inside_window(const WINDOW *win, int x, int y) {
  int x1, y1, x2, y2;
  getyx(win, y1, x1);
  getmaxyx(win, y2, x2);
  return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

static void build_combination_view(ui_t *ui) {
  int rows, cols;
  WINDOW *parent = ui->current_layout->scores.scorecard;

  getmaxyx(parent, rows, cols);

  int width = cols - 2;
  int available_rows = rows - 2;

  const int MIN_H = 1;
  const int MAX_H = 3;

  int row_height = available_rows / VIEW_COMBINATION_SIZE;
  if (row_height < MIN_H)
    row_height = MIN_H;
  if (row_height > MAX_H)
    row_height = MAX_H;

  int max_visible = available_rows / row_height;

  int y = 1;

  for (int i = 0; i < max_visible && i < VIEW_COMBINATION_SIZE; ++i) {
    ui->current_layout->scores.combination_view[i] =
        derwin(parent, row_height, width, y, 1);

    y += row_height;
  }
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

static void build_orizontal_layout(ui_t *ui, int rows, int cols) {
  del_layout(&ui->layout[LANDSCAPE]);

  const int title_h = 3;
  const int menu_h = 3;
  const int tab_h = 3;

  const int left_w = cols / 2;
  const int right_w = cols - left_w;

  ui->layout[LANDSCAPE].title = newwin(title_h, left_w, 0, 0);
  ui->layout[LANDSCAPE].field.win = newwin(rows - title_h, left_w, title_h, 0);

  ui->layout[LANDSCAPE].scores.win = newwin(rows - menu_h, right_w, 0, left_w);
  ui->layout[LANDSCAPE].menu.win =
      newwin(menu_h, right_w, rows - menu_h, left_w);

  box(ui->layout[LANDSCAPE].title, 0, 0);
  box(ui->layout[LANDSCAPE].field.win, 0, 0);
  box(ui->layout[LANDSCAPE].scores.win, 0, 0);
  box(ui->layout[LANDSCAPE].menu.win, 0, 0);

  ui->layout[LANDSCAPE].field.roll_button =
      derwin(ui->layout[LANDSCAPE].field.win, 3, 10, rows - title_h - 3, 0);

  wborder(ui->layout[LANDSCAPE].field.roll_button, 0, 0, 0, 0, ACS_LTEE, 0, 0,
          ACS_BTEE);

  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i)
    ui->layout[LANDSCAPE].menu.section[i] =
        derwin(ui->layout[LANDSCAPE].menu.win, menu_h, MENU_OPTION_WIDTH, 0,
               6 + MENU_OPTIONS_OFFSET(i));

  for (int i = 0; i < NUM_PLAYERS; ++i)
    ui->layout[LANDSCAPE].scores.player_tab[i] =
        derwin(ui->layout[LANDSCAPE].scores.win, tab_h, PLAYER_TAB_WIDTH, 0,
               PLAYER_TAB_OFFSET(i));

  ui->layout[LANDSCAPE].scores.scorecard =
      derwin(ui->layout[LANDSCAPE].scores.win, rows - tab_h - menu_h, right_w,
             tab_h, 0);

  wborder(ui->layout[LANDSCAPE].scores.scorecard, 0, 0, 0, 0, ACS_LTEE,
          ACS_RTEE, 0, 0);
  build_combination_view(ui);
}

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
static void build_vertical_layout(ui_t *ui, int rows, int cols) {
  del_layout(&ui->layout[PORTRAIT]);

  const int title_h = 3;
  const int menu_h = 3;
  const int tab_h = 3;
  const int roll_h = 3;

  int y = 0;

  /* TITLE */
  ui->layout[PORTRAIT].title = newwin(title_h, cols, y, 0);
  y += title_h;

  /* MENU */
  ui->layout[PORTRAIT].menu.win = newwin(menu_h, cols, y, 0);
  y += menu_h;

  int field_h = rows - (title_h + menu_h + tab_h + roll_h + 3);
  ui->layout[PORTRAIT].field.win = newwin(field_h, cols, y, 0);
  y += field_h;

  ui->layout[PORTRAIT].scores.win =
      newwin(tab_h + (rows - y - roll_h), cols, y, 0);

  /* PLAYER TABS */
  for (int i = 0; i < NUM_PLAYERS; ++i)
    ui->layout[PORTRAIT].scores.player_tab[i] =
        derwin(ui->layout[PORTRAIT].scores.win, tab_h, 16, 0, i * 16);

  ui->layout[PORTRAIT].scores.scorecard =
      derwin(ui->layout[PORTRAIT].scores.win,
             getmaxy(ui->layout[PORTRAIT].scores.win) - tab_h, cols, tab_h, 0);

  y += getmaxy(ui->layout[PORTRAIT].scores.win);

  ui->layout[PORTRAIT].field.roll_button =
      newwin(roll_h, cols, rows - roll_h, 0);

  /* BOX */
  box(ui->layout[PORTRAIT].title, 0, 0);
  box(ui->layout[PORTRAIT].menu.win, 0, 0);
  box(ui->layout[PORTRAIT].field.win, 0, 0);
  box(ui->layout[PORTRAIT].scores.win, 0, 0);
  box(ui->layout[PORTRAIT].field.roll_button, 0, 0);

  /* MENU SECTIONS */
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i)
    ui->layout[PORTRAIT].menu.section[i] =
        derwin(ui->layout[PORTRAIT].menu.win, menu_h, cols / MENU_OPTIONS_SIZE,
               0, i * (cols / MENU_OPTIONS_SIZE));
}

static int sum_upper_combinations(const scorecard_t *card) {
  int sum = 0;
  for (int i = 0; i < UPPER_SIZE; ++i) {
    if (card->upper[i].selected)
      sum += card->upper[i].points;
  }
  return sum;
}

static int *get_combination_view_values(ui_t *ui) {
  static int values[VIEW_COMBINATION_SIZE] = {0};
  player_t *p = &ui->yahtzee->player[ui->current_layout->scores.scorecard_view];

  int total_upper_combinations = 0;
  if (!yahtzee_is_there_unselected_upper_combination(&p->card))
    total_upper_combinations = sum_upper_combinations(&p->card);

  for (int i = 0; i < UPPER_SIZE; ++i)
    values[i] = p->card.upper[i].points;

  for (int i = 0; i < LOWER_SIZE; ++i)
    values[i + VIEW_CHANCE] = p->card.lower[i].points;

  values[VIEW_TOTAL] = p->total_score;
  return values;
}

static void draw_combination_view(ui_t *ui) {
  int rows, cols;
  getmaxyx(ui->current_layout->scores.combination_view[0], rows, cols);

  const int display_h = rows - 1;

  int *values = get_combination_view_values(ui);

  for (int i = 0; i < VIEW_COMBINATION_SIZE; ++i) {
    mvwprintw(ui->current_layout->scores.combination_view[i], display_h, 1,
              "%-20s  %4d", combination_view_name(i), values[i]);
  }
}

static void draw_layout(ui_t *ui) {
  mvwprintw(ui->current_layout->title, 1, 2, "Yahtzee");

  mvwprintw(ui->current_layout->field.roll_button, 1, 1, "Roll (%d)",
            ui->yahtzee->attempts);

  mvwprintw(ui->current_layout->field.win, 1, 1, "Turn: Player %d",
            ui->yahtzee->active_player + 1);
  mvwprintw(ui->current_layout->menu.win, 1, 1, "Menu: ");
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i)
    mvwprintw(ui->current_layout->menu.section[i], 1, 1, "%s",
              menu_option_name(i));

  for (int i = 0; i < NUM_PLAYERS; ++i)
    mvwprintw(ui->current_layout->scores.player_tab[i], 1, 1, "Player %d",
              i + 1);

  draw_combination_view(ui);
}

static void build_layout(ui_t *ui, int rows, int cols) {
  if (is_layout_active(ui, LANDSCAPE)) {
    build_orizontal_layout(ui, rows, cols);
  } else if (is_layout_active(ui, PORTRAIT)) {
    build_vertical_layout(ui, rows, cols);
  }
}

static void refresh_layout(ui_t *ui) {
  wnoutrefresh(ui->current_layout->title);
  wnoutrefresh(ui->current_layout->field.win);
  wnoutrefresh(ui->current_layout->field.roll_button);
  wnoutrefresh(ui->current_layout->menu.win);
  wnoutrefresh(ui->current_layout->scores.win);

  for (int i = 0; i < NUM_DICES; ++i)
    wnoutrefresh(ui->current_layout->field.dice[i]);

  for (int i = 0; i < NUM_PLAYERS; ++i)
    wnoutrefresh(ui->current_layout->scores.player_tab[i]);

  wnoutrefresh(ui->current_layout->scores.scorecard);

  for (int i = 0; i < VIEW_COMBINATION_SIZE; ++i)
    wnoutrefresh(ui->current_layout->scores.combination_view[i]);

  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i)
    wnoutrefresh(ui->current_layout->menu.section[i]);

  doupdate();
}

click_target_t ui_get_click_target(ui_t *ui, int x, int y) {
  click_target_t target = {TARGET_NONE, -1};
  layout_t *l = ui->current_layout; // Usa il layout attivo

  // 1. Check Roll Button
  if (is_mouse_inside_window(l->field.roll_button, x, y)) {
    target.type = TARGET_ROLL_BUTTON;
    return target;
  }

  // 2. Check Dices
  for (int i = 0; i < NUM_DICES; ++i) {
    if (is_mouse_inside_window(l->field.dice[i], x, y)) {
      target.type = TARGET_DICE;
      target.index = i;
      return target;
    }
  }

  // 3. Check Categories (Combination View)
  for (int i = 0; i < VIEW_COMBINATION_SIZE; ++i) {
    // Nota: combination_view[i] potrebbe essere NULL se la finestra è troppo
    // piccola, aggiungere check
    if (l->scores.combination_view[i] &&
        is_mouse_inside_window(l->scores.combination_view[i], x, y)) {
      target.type = TARGET_CATEGORY;
      target.index = i;
      return target;
    }
  }

  // 4. Check Menu
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    if (is_mouse_inside_window(l->menu.section[i], x, y)) {
      target.type = TARGET_MENU_OPTION;
      target.index = i;
      return target;
    }
  }

  // 5. Check Player Tabs
  for (int i = 0; i < NUM_PLAYERS; ++i) {
    if (is_mouse_inside_window(l->scores.player_tab[i], x, y)) {
      target.type = TARGET_PLAYER_TAB;
      target.index = i;
      return target;
    }
  }

  return target;
}

ui_t *ui_init(yahtzee_t *y) {
  ui_t *ui = malloc(sizeof(ui_t));

  ui->yahtzee = y;
  ui->layout[LANDSCAPE] = (layout_t){0};
  ui->layout[PORTRAIT] = (layout_t){0};

  // initializetions ncurses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

  nodelay(stdscr, TRUE); // getch() will not wait for input

  choose_appropriate_layout(ui);

  if (has_colors()) {
    start_color();
    use_default_colors();
  }

  refresh();

  return ui;
}

void ui_draw(ui_t *ui) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  build_layout(ui, rows, cols);
  draw_layout(ui);
  refresh_layout(ui);
}

void ui_free(ui_t **ui) {
  del_layout(&(*ui)->layout[LANDSCAPE]);
  del_layout(&(*ui)->layout[PORTRAIT]);

  endwin();
  free(*ui);
  *ui = NULL;
}
