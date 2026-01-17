#ifndef INPUT_H
#define INPUT_H

#include "ui.h"

// Tipi di eventi che il gioco può gestire
typedef enum {
  EVENT_NONE,
  EVENT_QUIT,
  EVENT_RESIZE,
  EVENT_ROLL,
  EVENT_CLICK,
  EVENT_HOVER,
} event_type;

// Struttura che contiene l'evento e dati opzionali (es. quale dado)
typedef struct {
  event_type type;
  ui_target_t target;
} game_event_t;

// Funzione principale per ottenere il prossimo evento
game_event_t input_get_event(ui_t *ui);

#endif // INPUT_H
