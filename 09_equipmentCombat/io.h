#ifndef IO_H
# define IO_H

class dungeon;

void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(dungeon *d);
void io_handle_input(dungeon *d);
void io_queue_message(const char *format, ...);
void io_convertObject(object *o, char *c, uint32_t size);
void io_wear_item(dungeon *d);
void io_remove_item(dungeon*d);
void io_drop_item(dungeon *d);
void io_permanent_itemRemoval(dungeon *d);
void io_display_inventory(dungeon *d);
void io_display_equip(dungeon *d);

#endif
