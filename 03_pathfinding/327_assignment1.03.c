#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rlg327.h"

void gen_distances(dungeon_t*);
void pc(dungeon_t d, int pcX, int pcY)

// FOR BOTH MAPS -1 SIGNIFIES THE PC
int nt_dist[DUNGEON_Y][DUNGEON_X]; // int array that works as the distance map for non-tunneling -2 IS WALL
int t_dist[DUNGEON_Y][DUNGEON_X]; // int array for the distance map for tunnelling monsters
// for all cells
    // d -> nt_dist[i][j] = paths[i][j].cost;

/**
    Notes:
      - Need to use Djistka's Algorithm
      - Creating path finding algorithms for monsters
          - some can go through walls
          - some can only move through open space
      - Output: "Your submission, when run, should generate a dungeon, calculate all distance maps, render all three
        views of the dungeon (the “standard” view and the two distance maps), and exit."
      - Method: "A na¨ıve implementation will call pathfinding for every monster in the dungeon, but in practice, every
        monster is trying to get to the same place, so rather than calculating paths from the monsters to the player
        character (PC), we can instead calculate the distance from the PC to every point in the dungeon, and this
        only needs to be updated when the PC moves or the dungeon changes. Each monster will choose to move to
        the neighboring cell with the lowest distance to PC."
        - have a method that calcs distance to every point that calls dijkstrasMethod
        - printDungeon view that prints all three views
      - Probably should use his code for 1.01 & 1.02
**/

int main(int argc, char* argv[]) {
    dungeon_t *d;
    // generate a dungeon
    if (argc == 1)
      gen_dungeon(&d);
    else if (argv[2] == "--load") {
      // call load
    }

    // calculate all distance maps
    gen_distances(&d);

    // print dungeon layouts
    // std dungeon
    render_dungeon(&d);
    // ntunn and tunn dist maps
    print_maps();
}

// method to read map int arrays for gen_distances
void print_maps() {
  int x, y;

  // print non tunnelling monsters dist map rendering pc using @ and distances from pc modulo 10
  for (y = 0; y <= 21; y++) {
    for (x = 0; x<= 81; x++) {
      // if its the spot with the pc render @
      if (nt_dist[y][x] == -1) {
        printf("@");
      }
      // if its -2, then its a wall
      else if (nt_dist[y][x] == -2) {
        printf(" ");
      }
      // else mod 10 the distance to print dist last digit
      else if (nt_distance != NULL) {
        printf("%d", nt_dist[y][x] % 10);
      }
    }
    printf("\n");
  }

  // print tunneling monster distance map encoded in same manner as non
  for (y = 0; y <= 21; y++) {
    for (x = 0; x<= 81; x++) {
      // if its the spot with the pc render @
      if (t_dist[y][x] == -1) {
        printf("@");
      }
      // if its a ter_wall_immutable
      else if (dungeon.map[y][x] == ter_wall_immutable) {
        printf(" ");
      }
      // print dist to pc
      else if (t_dist[x][y] != NULL){
        printf("%d", t_dist[y][x] % 10);
      }
    }
    printf("\n");
  }
}

// places the pc (@) in a random cell that is a floor (corridor or room)
void pc(dungeon_t *d) {
  bool placePC = false;
  int x, y;

  while(!placePC) {
    x = (rand() % 80) + 1;
    y = (rand() % 20) + 1;
    if(d[x][y] == ter_floor) {
      dungeon.pc[dim_x] = x;
      dungeon.pc[dim_y] = y;
      placePC = true;
    }
  }
}

// method that calcs distance to every point that calls dijkstrasMethod
void gen_distances(dungeon_t *d, int pcX, int pcY) {
    int i, j;
    // render pc at random spot
    pc(&d);
}

static int32_t dijkstra_nontunnel(const void *key, const void *with) {
  return ((dijkstra_nontunnel*) key) ->cost - ((dijkstra_nontunnel*) with) ->cost;
}

//FOR MONSTERS WHO CAN'T TUNNEL
static void dijkstra_nontunnel(dungeon_t *d)  {
  static monster_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  //going to change so its not focusing on corridors
  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] +  1]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + 1;
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + 1;
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
  }
  if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
      (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
       p->cost + 1)) {
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
      p->cost + 1;
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
    heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                         [p->pos[dim_x] + 1].hn);
}    for (i = 1; i < DUNGEON_Y; i++) { // 1-20 x, 1-80 y?? for borders
    for (j = 1; j < DUNGEON_X; j++) {
      // gen dists for those who cannot tunnel (thru wall) ONLY SEARCH POINTS THAT ARE HARDNESS 0??
      if (y == pcY && x == pcX){
        printf("%c", '@');
      } else if(path[y][x].cost != INT_MAX) {
        printf("%d", path[y][x].cost % 10);
      } else {
        printf("%c", ' ');
      }
      printf("\n");
    }
  }
 }
}

static int32_t dijkstra_tunnel(const void *key, const void *with) {
  return ((dijkstra_nontunnel*) key) ->cost - ((dijkstra_nontunnel*) with) ->cost;
}


//FOR MONSTERS WHO CAN TUNNEL
static void dijkstra_tunnel(dungeon_t *d)  {
  static monster_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  //going to change so its not focusing on corridors
  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] +  1]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
        p->cost + 1;
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + 1)) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + 1;
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + 1)) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + 1;
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
  }
  if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
      (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
       p->cost + 1)) {
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
      p->cost + 1;
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
    path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
    heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                         [p->pos[dim_x] + 1].hn);
}    for (i = 1; i < DUNGEON_Y; i++) { // 1-20 x, 1-80 y?? for borders
    for (j = 1; j < DUNGEON_X; j++) {
      // gen dists for those who cannot tunnel (thru wall) ONLY SEARCH POINTS THAT ARE HARDNESS 0??
      if (y == pcY && x == pcX){
        printf("%c", '@');
      } else if(path[y][x].cost != INT_MAX) {
        printf("%d", path[y][x].cost % 10);
      } else {
        printf("%c", ' ');
      }
      printf("\n");
    }
  }
 }
}
