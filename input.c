#include "input.h"

game_event_t input_get_event(ui_t *ui) {
  game_event_t event = {EVENT_NONE, 0};
  int ch = getch();

  if (ch == ERR)
    return event;

  // --- KEYBOARD HANDLING ---
  switch (ch) {
  case 'q':
  case 'Q':
    event.type = EVENT_QUIT;
    return event;
  case 'r':
  case 'R':
    event.type = EVENT_ROLL;
    return event;
  case KEY_RESIZE:
    event.type = EVENT_RESIZE;
    return event;
  // Selezione dadi con tasti numerici 1-5
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
    event.type = EVENT_TOGGLE_DICE;
    event.index = ch - '1';
    return event;
  case KEY_MOUSE: {
    MEVENT mouse_event;
    if (getmouse(&mouse_event) == OK) {
      // Gestiamo solo il click sinistro (o tocco)
      if (mouse_event.bstate & (BUTTON1_CLICKED | BUTTON1_PRESSED)) {
        // Chiediamo alla UI cosa abbiamo colpito
        click_target_t target =
            ui_get_click_target(ui, mouse_event.x, mouse_event.y);

        switch (target.type) {
        case TARGET_ROLL_BUTTON:
          event.type = EVENT_ROLL;
          break;
        case TARGET_DICE:
          event.type = EVENT_TOGGLE_DICE;
          event.index = target.index;
          break;
        case TARGET_CATEGORY:
          event.type = EVENT_SELECT_CATEGORY;
          event.index = target.index; // Indice della view (ACES, TWOS...)
          break;
        case TARGET_MENU_OPTION:
          if (target.index == QUIT)
            event.type = EVENT_QUIT;
          // if (target.index == HELP) ...
          break;
        case TARGET_PLAYER_TAB:
          event.type = EVENT_SWITCH_TAB;
          event.index = target.index;
          break;
        default:
          break;
        }
      }
    }
    return event;
  }
  }

  return event;
}
