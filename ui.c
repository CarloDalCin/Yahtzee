#include "ui.h"
#include "yahtzee.h"

#define PLAYER_TAB_WIDTH 14
#define PLAYER_TAB_OFFSET(player) ((player) * PLAYER_TAB_WIDTH)
#define MENU_OPTION_WIDTH 8
#define MENU_OPTIONS_OFFSET(option) ((option) * MENU_OPTION_WIDTH)

// choose layout base on terminal size
void ui_choose_appropriate_layout(ui_t *ui) {
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

void ui_change_player_tab(scores_t *s, int8_t view) {
  s->scorecard_view = view % NUM_PLAYERS;
}

int ui_get_next_player_tab_view(scores_t *s) {
  ui_change_player_tab(s, s->scorecard_view + 1);
  return s->scorecard_view;
}

int ui_get_previous_player_tab_view(scores_t *s) {
  ui_change_player_tab(s, s->scorecard_view - 1 + NUM_PLAYERS);
  return s->scorecard_view;
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
  if (!l)
    return;
  del_win(&l->title);
  del_field(&l->field);
  del_menu(&l->menu);
  del_scores(&l->scores);
}

bool ui_is_mouse_inside_window(const WINDOW *win, int x, int y) {
  if (!win)
    return false;
  int beg_y, beg_x, max_y, max_x;
  getbegyx(win, beg_y, beg_x); // Origin
  getmaxyx(win, max_y, max_x); // Dimension
  return (x >= beg_x && x < beg_x + max_x && y >= beg_y && y < beg_y + max_y);
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

  for (int i = VIEW_ACES; i < max_visible && i < VIEW_COMBINATION_SIZE; ++i) {
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

  const int dice_w = 9, dice_h = 5;
  const int start_y = (rows - title_h) / 2 - 2; // Centrato verticalmente
  const int start_x = 2;

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

  for (int i = 0; i < NUM_DICES; ++i) {
    ui->layout[LANDSCAPE].field.dice[i] =
        derwin(ui->layout[LANDSCAPE].field.win, dice_h, dice_w, start_y,
               start_x + (i * (dice_w + 1)));
  }

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

  const int field_h = rows - (title_h + menu_h + tab_h + roll_h + 3);

  const int dice_w = 9, dice_h = 5;
  const int start_y = field_h / 2 - 2;
  int start_x = (cols - (NUM_DICES * dice_w) - ((NUM_DICES - 1) * 1)) / 2;
  if (start_x < 1)
    start_x = 1;

  int y = 0;

  /* TITLE */
  ui->layout[PORTRAIT].title = newwin(title_h, cols, y, 0);
  y += title_h;

  /* MENU */
  ui->layout[PORTRAIT].menu.win = newwin(menu_h, cols, y, 0);
  y += menu_h;

  /* FIELD */
  ui->layout[PORTRAIT].field.win = newwin(field_h, cols, y, 0);
  y += field_h;

  /* DICES */
  for (int i = 0; i < NUM_DICES; ++i) {
    ui->layout[PORTRAIT].field.dice[i] =
        derwin(ui->layout[PORTRAIT].field.win, dice_h, dice_w, start_y,
               start_x + (i * (dice_w + 1)));
  }

  /* SCORECARD */
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
  for (int i = VIEW_ACES; i < MENU_OPTIONS_SIZE; ++i)
    ui->layout[PORTRAIT].menu.section[i] =
        derwin(ui->layout[PORTRAIT].menu.win, menu_h, cols / MENU_OPTIONS_SIZE,
               0, i * (cols / MENU_OPTIONS_SIZE));
}

static int sum_upper_combinations(const scorecard_t *card) {
  int sum = 0;
  for (int i = ACES; i < UPPER_SIZE; ++i) {
    if (card->upper[i].selected)
      sum += card->upper[i].points;
  }
  return sum;
}

lower_section ui_combination_view_to_lower_section(combination_view view) {
  if (view >= VIEW_CHANCE && view <= VIEW_YAHTZEE) {
    return (lower_section)(view - VIEW_CHANCE);
  }
  return -1;
}

upper_section ui_combination_view_to_upper_section(combination_view view) {
  if (view >= VIEW_ACES && view <= VIEW_SIXES) {
    return (upper_section)(view - VIEW_ACES);
  }
  return -1;
}

static int *get_combination_view_values(ui_t *ui) {
  static int values[VIEW_COMBINATION_SIZE] = {0};
  player_t *p = &ui->yahtzee->player[ui->current_layout->scores.scorecard_view];

  int total_upper_combinations = 0;
  if (!yahtzee_is_there_unselected_upper_combination(&p->card))
    total_upper_combinations = sum_upper_combinations(&p->card);

  for (int i = ACES; i < UPPER_SIZE; ++i)
    values[i] = p->card.upper[i].points;

  for (int i = CHANCE; i < LOWER_SIZE; ++i)
    values[i + VIEW_CHANCE] = p->card.lower[i].points;

  values[VIEW_UPPER_BONUS] = p->card.upper_bonus;
  values[VIEW_TOTAL] = p->total_score;
  return values;
}

static void draw_combination_view(ui_t *ui) {
  int *values = get_combination_view_values(ui);
  player_t *p = &ui->yahtzee->player[ui->current_layout->scores.scorecard_view];

  for (int i = VIEW_ACES; i < VIEW_COMBINATION_SIZE; ++i) {
    WINDOW *row_win = ui->current_layout->scores.combination_view[i];
    if (!row_win)
      continue;

    bool is_hovered = (ui->hovered_element.type == ELEMENT_CATEGORY &&
                       ui->hovered_element.index == i);

    int current_color = CP_DEFAULT;
    if (is_hovered) {
      current_color = CP_HOVER;
    }

    wbkgd(row_win, COLOR_PAIR(current_color));
    werase(row_win);

    bool is_selected = false;
    if (i >= VIEW_ACES && i <= VIEW_SIXES) {
      is_selected = p->card.upper[i - VIEW_ACES].selected;
    } else if (i >= VIEW_CHANCE && i <= VIEW_YAHTZEE) {
      is_selected = p->card.lower[i - VIEW_CHANCE].selected;
    }

    if (i == VIEW_UPPER_BONUS || i == VIEW_TOTAL) {
      mvwprintw(row_win, 0, 1, "    %-16s  %4d", combination_view_name(i),
                values[i]);
    } else if (is_selected) {
      mvwprintw(row_win, 0, 1, "[X] %-16s  %4d", combination_view_name(i),
                values[i]);
    } else {
      mvwprintw(row_win, 0, 1, "[ ] %-16s  %4d", combination_view_name(i),
                values[i]);
    }
  }
}

static void draw_dice_face(WINDOW *win, int value) {
  if (!win)
    return;

  // Empty the dice
  for (int y = 1; y < 4; y++) {
    for (int x = 1; x < 8; x++) {
      mvwaddch(win, y, x, ' ');
    }
  }

  // The cordinates are relative to the dice window (which is high 5 and wide 9)
  // Center: y=2, x=4
  // Top Sx: y=1, x=2  | Top Dx: y=1, x=6
  // Bottom Sx: y=3, x=2 | Bottom Dx: y=3, x=6
  // Center Sx: y=2, x=2 | Center Dx: y=2, x=6

  switch (value) {
  case 1:
    mvwprintw(win, 2, 4, "o"); // Center
    break;
  case 2:
    mvwprintw(win, 1, 2, "o"); // Top Sx
    mvwprintw(win, 3, 6, "o"); // Bottom Dx
    break;
  case 3:
    mvwprintw(win, 1, 2, "o"); // Top Sx
    mvwprintw(win, 2, 4, "o"); // Center
    mvwprintw(win, 3, 6, "o"); // Bottom Dx
    break;
  case 4:
    mvwprintw(win, 1, 2, "o"); // Top Sx
    mvwprintw(win, 1, 6, "o"); // Top Dx
    mvwprintw(win, 3, 2, "o"); // Bottom Sx
    mvwprintw(win, 3, 6, "o"); // Bottom Dx
    break;
  case 5:
    mvwprintw(win, 1, 2, "o"); // Top Sx
    mvwprintw(win, 1, 6, "o"); // Top Dx
    mvwprintw(win, 2, 4, "o"); // Center
    mvwprintw(win, 3, 2, "o"); // Bottom Sx
    mvwprintw(win, 3, 6, "o"); // Bottom Dx
    break;
  case 6:
    mvwprintw(win, 1, 2, "o"); // Top Sx
    mvwprintw(win, 1, 6, "o"); // Top Dx
    mvwprintw(win, 2, 2, "o"); // Center Sx
    mvwprintw(win, 2, 6, "o"); // Center Dx
    mvwprintw(win, 3, 2, "o"); // Bottom Sx
    mvwprintw(win, 3, 6, "o"); // Bottom Dx
    break;
  case 0: // Dice not launched yet
    mvwprintw(win, 2, 4, "?");
    break;
  }
}

static void draw_layout(ui_t *ui) {
  layout_t *l = ui->current_layout;

  // ==========================================
  // 1. TITLE
  // ==========================================
  wattron(l->title, COLOR_PAIR(CP_TITLE) | A_BOLD);
  box(l->title, 0, 0);
  mvwprintw(l->title, 1, 2, "Yahtzee");
  wattroff(l->title, COLOR_PAIR(CP_TITLE) | A_BOLD);

  // ==========================================
  // 2. FIELD
  // ==========================================
  box(l->field.win, 0, 0);
  mvwprintw(l->field.win, 1, 1, "Turn: Player %d",
            ui->yahtzee->active_player + 1);

  // ==========================================
  // 3. ROLL BUTTON
  // ==========================================
  int roll_color =
      (ui->hovered_element.type == ELEMENT_ROLL_BUTTON) ? CP_HOVER : CP_DEFAULT;
  wattron(l->field.roll_button, COLOR_PAIR(roll_color));

  // Disegna bordo (Gestione Landscape vs Portrait)
  if (is_layout_active(ui, LANDSCAPE)) {
    wborder(l->field.roll_button, 0, 0, 0, 0, ACS_LTEE, 0, 0, ACS_BTEE);
  } else {
    box(l->field.roll_button, 0, 0);
  }

  mvwprintw(l->field.roll_button, 1, 1, "Roll (%d)", ui->yahtzee->attempts);
  wattroff(l->field.roll_button, COLOR_PAIR(roll_color));

  // ==========================================
  // 4. MENU SECTIONS (Help, Quit)
  // ==========================================
  box(l->menu.win, 0, 0);
  mvwprintw(l->menu.win, 1, 1, "Menu: ");

  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    int menu_color = (ui->hovered_element.type == ELEMENT_MENU_OPTION &&
                      ui->hovered_element.index == i)
                         ? CP_HOVER
                         : CP_DEFAULT;

    wattron(l->menu.section[i], COLOR_PAIR(menu_color));
    if (menu_color == CP_HOVER)
      wbkgd(l->menu.section[i], COLOR_PAIR(CP_HOVER));
    else
      wbkgd(l->menu.section[i], COLOR_PAIR(CP_DEFAULT));

    mvwprintw(l->menu.section[i], 1, 1, "%s", menu_option_name(i));
    wattroff(l->menu.section[i], COLOR_PAIR(menu_color));
  }

  // ==========================================
  // 5. PLAYER TABS
  // ==========================================
  box(l->scores.win, 0, 0);

  for (int i = 0; i < NUM_PLAYERS; ++i) {
    int tab_color = CP_DEFAULT;

    if (ui->hovered_element.type == ELEMENT_PLAYER_TAB &&
        ui->hovered_element.index == i) {
      tab_color = CP_HOVER;
    } else if (i == ui->current_layout->scores.scorecard_view) {
      tab_color = CP_SELECTED; // Highlight current player tab
    }

    wattron(l->scores.player_tab[i], COLOR_PAIR(tab_color));
    box(l->scores.player_tab[i], 0, 0);
    mvwprintw(l->scores.player_tab[i], 1, 1, "Player %d", i + 1);
    wattroff(l->scores.player_tab[i], COLOR_PAIR(tab_color));
  }

  // Bordo Scorecard interno
  if (is_layout_active(ui, LANDSCAPE)) {
    wborder(l->scores.scorecard, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);
  } else {
    box(l->scores.scorecard, 0, 0);
  }

  // ==========================================
  // 6. DICES
  // ==========================================
  for (int i = 0; i < NUM_DICES; ++i) {
    WINDOW *d_win = ui->current_layout->field.dice[i];
    if (!d_win)
      continue;

    // 1. Determina il colore del dado (Hover, Selezionato o Default)
    int dice_color = CP_DEFAULT;
    if (ui->hovered_element.type == ELEMENT_DICE &&
        ui->hovered_element.index == i) {
      dice_color = CP_HOVER;
    } else if (ui->yahtzee->dice[i].selected == SELECTED) {
      dice_color = CP_SELECTED;
    }

    // 2. Applica il colore
    wattron(d_win, COLOR_PAIR(dice_color));

    // Disegna il bordo del dado con il colore corretto
    box(d_win, 0, 0);

    // Piccolo indice in alto a sinistra (es. [1], [2]) per far capire quale
    // tasto premere da tastiera
    mvwprintw(d_win, 0, 1, "[%d]", i + 1);

    // 3. DISEGNA LA FACCIA DEL DADO (La magia avviene qui!)
    draw_dice_face(d_win, ui->yahtzee->dice[i].value);

    // 4. Scritta "HOLD" se il dado è trattenuto
    if (ui->yahtzee->dice[i].selected == SELECTED) {
      // Scriviamo "HOLD" in basso al centro. Larghezza dado = 9. Centro =
      // (9-4)/2 = 2
      mvwprintw(d_win, 4, 2, "HOLD");
    } else {
      // Pulisci lo spazio se viene deselezionato
      mvwprintw(d_win, 4, 2, "    ");
    }

    // 5. Spegni il colore
    wattroff(d_win, COLOR_PAIR(dice_color));
  }

  // ==========================================
  // 7. COMBINATION VIEW
  // ==========================================
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

ui_target_t ui_pick_target(ui_t *ui, int x, int y) {
  ui_target_t target = {ELEMENT_NONE, -1};
  layout_t *l = ui->current_layout; // Usa il layout attivo

  // 1. Check Roll Button
  if (ui_is_mouse_inside_window(l->field.roll_button, x, y)) {
    target.type = ELEMENT_ROLL_BUTTON;
    return target;
  }

  // 2. Check Dices
  for (int i = 0; i < NUM_DICES; ++i) {
    if (ui_is_mouse_inside_window(l->field.dice[i], x, y)) {
      target.type = ELEMENT_DICE;
      target.index = i;
      return target;
    }
  }

  // 3. Check Categories (Combination View)
  for (int i = VIEW_ACES; i < VIEW_COMBINATION_SIZE; ++i) {
    WINDOW *cat_win = l->scores.combination_view[i];
    if (cat_win) {
      // ui_is_mouse_inside_window DEVE usare getbegyx
      if (ui_is_mouse_inside_window(cat_win, x, y)) {
        target.type = ELEMENT_CATEGORY;
        target.index = i;
        return target;
      }
    }
  }

  // 4. Check Menu
  for (int i = 0; i < MENU_OPTIONS_SIZE; ++i) {
    if (ui_is_mouse_inside_window(l->menu.section[i], x, y)) {
      target.type = ELEMENT_MENU_OPTION;
      target.index = i;
      return target;
    }
  }

  // 5. Check Player Tabs
  for (int i = 0; i < NUM_PLAYERS; ++i) {
    if (ui_is_mouse_inside_window(l->scores.player_tab[i], x, y)) {
      target.type = ELEMENT_PLAYER_TAB;
      target.index = i;
      return target;
    }
  }

  return target;
}

// In ui.c
static void init_color_pairs(ui_t *ui) {
  if (has_colors()) {
    start_color();
    use_default_colors(); // Allow to use -1 as transparent color

    // init_pair(ID_PAIR, TEXT_COLOR, BACKGROUND_COLOR)

    // Default: Text White, Background Trasparent
    init_pair(CP_DEFAULT, COLOR_WHITE, -1);

    // Selected: Text White, Background Blu
    init_pair(CP_SELECTED, COLOR_WHITE, COLOR_BLUE);

    // Hover: Text Black, Background Cyan
    init_pair(CP_HOVER, COLOR_BLACK, COLOR_CYAN);

    // Titolo: Testo Magenta, Sfondo trasparente
    // Title: Text Magenta, Background Transparent
    init_pair(CP_TITLE, COLOR_MAGENTA, -1);
  }
}

void ui_roll_animation(ui_t *ui) {
  // TODO! add animation

  int rows, cols;
  const int SIDE = 3;
  getmaxyx(ui->current_layout->field.win, rows, cols);

  yahtzee_roll_dices(ui->yahtzee);

  for (int i = 0; i < NUM_DICES; ++i) {
    if (ui->yahtzee->dice[i].selected == UNSELECTED) {
      del_win(&ui->current_layout->field.dice[i]);
      // TODO refresh only the dices
      ui_draw(ui);

      ui->current_layout->field.dice[i] =
          derwin(ui->current_layout->field.win, rows + i * SIDE + 1,
                 cols + i * SIDE + 1, SIDE, SIDE);
      box(ui->current_layout->field.dice[i], 0, 0);
      mvwprintw(ui->current_layout->field.dice[i], 0, 0, "%d", i + 1);
      mvwprintw(ui->current_layout->field.dice[i], 1, 1, "%d",
                ui->yahtzee->dice[i].value);
      ui_draw(ui);
    }
  }
}

ui_t *ui_init(yahtzee_t *y) {
  ui_t *ui = malloc(sizeof(ui_t));

  ui->current_layout = NULL;

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

  timeout(25); // 25 ms -> 40 fps

  ui_choose_appropriate_layout(ui);
  init_color_pairs(ui);

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
