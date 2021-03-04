#ifndef ASSIGNMENT_104_H
#define ASSIGNMENT_104_H

struct tunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    int32_t cost;
} tunnel_t;

struct nontunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    int32_t cost;
} nontunnel_t;

typedef struct m_path {
    heap_node_t *hn;
    uint8_t pos[2];
} mp_t;

char lose_message[12] = "you lost lol";
char win_message[28] = "you won. which is surprising";

void generate_monsters(dungeon_t *d, char m);

bool game_done(dungeon_t *d);

void straight(monster_t *monster, dungeon_t *d);

int in_line_of_sight(dungeon_t *d, monster_t *monster);

void shortest_path(monster_t* monster, dungeon_t* d);

void tunneling_hardness(dungeon_t* d, int x, int y);

void move(monster_t *monster, dungeon_t *d, heap_t *heap);

void combat(dungeon_t *d, monster_t *m);

void run_turns(dungeon_t *d);

#endif
