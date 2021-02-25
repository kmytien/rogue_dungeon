#ifndef RLG327_H
#define RLG327_H

#include "heap.h"
#include <stdio.h>

#define DUMP_HARDNESS_IMAGES 0

/* Returns true if random float in [0,1] is less than *
 * numerator/denominator.  Uses only integer math.    */
# define rand_under(numerator, denominator) \
  (rand() < ((RAND_MAX / denominator) * numerator))

/* Returns random integer in [min, max]. */
# define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))
# define UNUSED(f) ((void) f)

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

typedef struct dijkstra_nontunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} dijkstra_nontunnel_t;


typedef struct dijkstra_tunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} dijkstra_tunnel_t;


typedef struct corridor_path {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} corridor_path_t;


typedef struct monster_path {
    heap_node_t *hn;
    uint8_t pos[2];
    uint32_t cost;
} monster_path_t;


typedef enum dim {
    dim_x,
    dim_y,
    num_dims
} dim_t;

typedef int8_t pair_t[num_dims];

#define DUNGEON_X              80
#define DUNGEON_Y              21
#define MIN_ROOMS              6
#define MAX_ROOMS              10
#define ROOM_MIN_X             4
#define ROOM_MIN_Y             3
#define ROOM_MAX_X             20
#define ROOM_MAX_Y             15
#define SAVE_DIR               ".rlg327"
#define DUNGEON_SAVE_FILE      "dungeon"
#define DUNGEON_SAVE_SEMANTIC  "RLG327-S2021" //TERM
#define DUNGEON_SAVE_VERSION   0U


#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])
#define tunnel_cost(pair) (1 + (d->hardness[pair[dim_y]][pair[dim_x]]/ 85))


typedef enum __attribute__ ((__packed__)) terrain_type {
    ter_debug,
    ter_wall,
    ter_wall_immutable,
    ter_floor,
    ter_floor_room,
    ter_floor_hall,
    ter_stairs,
    ter_stairs_up,
    ter_stairs_down
} terrain_type_t;


typedef struct room {
    pair_t position;
    pair_t size;
} room_t;


typedef struct dungeon {
    uint16_t num_rooms;
    room_t *rooms;
    terrain_type_t map[DUNGEON_Y][DUNGEON_X];

    /* Since hardness is usually not used, it would be expensive to pull it *
     * into cache every time we need a map cell, so we store it in a        *
     * parallel array, rather than using a structure to represent the       *
     * cells.  We may want a cell structure later, but from a performanace  *
     * perspective, it would be a bad idea to ever have the map be part of  *
     * that structure.  Pathfinding will require efficient use of the map,  *
     * and pulling in unnecessary data with each map cell would add a lot   *
     * of overhead to the memory system.                                    */

    uint8_t hardness[DUNGEON_Y][DUNGEON_X];
    uint8_t nontunnel[DUNGEON_Y][DUNGEON_X];
    uint8_t tunnel[DUNGEON_Y][DUNGEON_X];
    uint8_t nt_dist[DUNGEON_Y][DUNGEON_X];
    uint8_t t_dist[DUNGEON_Y][DUNGEON_X];
    pair_t pc;
} dungeon_t;


//DID NOT ADD static/unsigned int methods - not sure if we need to?
int gen_dungeon(dungeon_t *d);

void render_dungeon(dungeon_t *d);

void delete_dungeon(dungeon_t *d);

void init_dungeon(dungeon_t *d);

int write_dungeon_map(dungeon_t *d, FILE *f);

int write_rooms(dungeon_t *d, FILE *f);

int write_stairs(dungeon_t *d, FILE *f);

int makedirectory(char *dir);

int write_dungeon(dungeon_t *d, char *file);

int read_dungeon_map(dungeon_t *d, FILE *f);

int read_stairs(dungeon_t *d, FILE *f);

int read_rooms(dungeon_t *d, FILE *f);

int read_dungeon(dungeon_t *d, char *file);

int read_pgm(dungeon_t *d, char *pgm);

void usage(char *name);

#endif
