#ifndef ASSIGNMENT_106_H
# define ASSIGNMENT_106_H

#include "dungeon.h"

void init_maps(dungeon_t *d);
void update_sight(dungeon_t *d);
int update_maps(dungeon_t *d, pair_t *f, pair_t *t);

#endif
