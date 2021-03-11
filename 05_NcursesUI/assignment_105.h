#ifndef ASSIGNMENT_105_H
#define ASSIGNMENT_105_H

#include "dungeon.h"
#include "character.h"

int stairs(dungeon_t *d);

uint32_t pc_next_pos(dungeon_t *d, pair_t dir);

void create_monster_list(dungeon_t *d);

void scroll_monster_list(dungeon_t *d, character_t **monsters, uint32_t count);

void print_monster_list(dungeon_t *d, character_t **monsters, uint32_t begin, uint32_t end);

void display_monster_list (dungeon_t *d, character_t **monsters, uint32_t slot);

void display_render_dungeon(dungeon_t *d);

#endif
