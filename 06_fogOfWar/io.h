#ifndef IO_H
# define IO_H

// typedef struct dungeon dungeon_t;
#include "dungeon.h"

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display_nf(dungeon_t *d);
void io_display_f(dungeon_t *d);
void io_handle_input(dungeon_t *d);
void io_queue_message(const char *format, ...);

#endif
