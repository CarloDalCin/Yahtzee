#include "input.h"
#include "ui.h"

game_event_t input_get_event(ui_t *ui) {
  game_event_t event = {EVENT_NONE, {ELEMENT_NONE, -1}};
  int ch = getch();

  if (ch == ERR)
    return event;

  switch (ch) {
  case KEY_MOUSE: {
    MEVENT mouse_event;
    if (getmouse(&mouse_event) == OK) {
      // Pick target hovered element
      event.target = ui_pick_target(ui, mouse_event.x, mouse_event.y);

      // Handle click
      if (mouse_event.bstate & (BUTTON1_CLICKED | BUTTON1_PRESSED)) {
        event.type = EVENT_CLICK;
      }
      // Handle hover
      else if (mouse_event.bstate & REPORT_MOUSE_POSITION) {
        event.type = EVENT_HOVER;
      }
    }
    break;
  }

  case KEY_RESIZE:
    event.type = EVENT_RESIZE;
    break;

    // Keypress
  case 'q':
  case 'Q':
    event.type = EVENT_CLICK;
    event.target = (ui_target_t){ELEMENT_MENU_OPTION, MENU_QUIT};
    break;

    // Roll dices
  case 'r':
  case 'R':
    event.type = EVENT_CLICK;
    event.target.type = ELEMENT_ROLL_BUTTON;
    break;

    // Select dice
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
    event.type = EVENT_CLICK;
    event.target = (ui_target_t){ELEMENT_DICE, ch - '1'};
    break;

    // Select help option
  case 'h':
  case 'H':
    event.type = EVENT_CLICK;
    event.target = (ui_target_t){ELEMENT_MENU_OPTION, MENU_HELP};
    break;

    // Select player tab
  case '\t':
    event.type = EVENT_CLICK;
    event.target =
        (ui_target_t){ELEMENT_PLAYER_TAB,
                      ui_get_next_player_tab_view(&ui->current_layout->scores)};
    break;

  case KEY_BTAB:
    event.type = EVENT_CLICK;
    event.target = (ui_target_t){
        ELEMENT_PLAYER_TAB,
        ui_get_previous_player_tab_view(&ui->current_layout->scores)};
    break;
  }

  return event;
}
