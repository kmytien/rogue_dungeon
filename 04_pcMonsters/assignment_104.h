#ifndef ASSIGNMENT_104_H
#define ASSIGNMENT_104_H

#include "dungeon.h"
#include "heap.h"

typedef struct m_path {
    heap_node_t *hn;
    uint8_t pos[2];
} mp_t;

void generate_monsters(dungeon_t *d);

bool game_done(dungeon_t *d);

void straight(monster_t *monster, dungeon_t *d);

int in_line_of_sight(dungeon_t *d, monster_t *monster);

void shortest_path(monster_t* monster, dungeon_t* d);

void tunneling_hardness(dungeon_t* d, int x, int y);

void move(monster_t *monster, dungeon_t *d, heap_t *heap);

void combat(dungeon_t *d, monster_t *m);

void run_turns(dungeon_t *d);

#endif
