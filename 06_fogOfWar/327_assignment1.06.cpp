#include <stdlib.h>

#include "character.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "dungeon.h"

/**
  NOTES:
  - Convert everything to c++
    - make all files cpp files
    - create a c++ makefile
    - all major structs should be converted to classes
      - Make npc and pc inheritors of the character class
  - create another map that shows what the character has seen
    - character can see/remember a 3x3 radius around them
    - this includes monsters (ie IF they are in the remembered areas)
  - add FOW and teleporting debug commands
    - use f to toggle fog of war view (whatever that means)
    - use g to toggle teleport mode
      - press g then g, the movement keys will move a targeted pointer (*) and the second g teleports the pc to that location
      - press g then r, sends the pc to a random location
      *** can be teleported into rock (not immutable), however, if SURROUNDED by rock, cannot tunnel (unless they teleport)
  - need to make 2 maps -> map where stuff is visible for pc and map that has past terrain (terrain that pc has already stepped on)
  - need to fix all errors in converted cpp files
  ALL NEW CODE TO BE WRITTEN IN C++
  Files to be switched to C++ IM NOT SURE WHAT NEEDS TO BE CHANGED AND WHAT DOESNT (idk if the .h files need to be changed???)
        also none of the ones with DONE next to them have been tested, i can do that after everything is switched over :) -H
        - all c files are supposed to be cpp (besides heap) but i didnt test them yet (i think they should be fine tho) -MyT
  - makefile  DONE
  - character.c  DONE
  - character.h
  - dims.h
  - dungeon.c
  - dungeon.h
  - event.c  DONE
  - event.h
  - heap.c
  - heap.h
  - io.c
  - io.h
  - move.c
  - move.h
  - npc.c
  - npc.h
  - path.c
  - path.h
  - pc.c
  - pc.h
  - rlg327.c
  - utils.c  DONE
  - utils.h  DONE?
**/

/* CREATE NEW MAP */
// - create another map that shows what the character has seen
//     - character can see/remember a 3x3 radius around them
//     - this includes monsters (ie IF they are in the remembered areas)


//need to put these in dungeon.h
terrain_type discovered[DUNGEON_Y][DUNGEON_X];
uint8_t visible[DUNGEON_Y][DUNGEON_X];

//on the piazza, sheaffer said to start off by initializing terrain map to all rock
//then i initialized the visible map to 0 (meaning not visible)
void init_maps(dungeon_t *d) {
    int y, x;

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            discovered[y][x] = ter_wall; //this is rock right aka a space?
            visible[y][x] = 0; //nothing is visible right now
        }
    }
}


//radius of whats visible to pc
void update_sight(dungeon_t *d) {

    int i, j, pc_visual = 3;
    pair_t x, y, pc_location = d->pc->position; //storing starting (dim_x) and end point (dim_y) for x/y ranges

    //if pc is in the first 3 columns
    if (d->pc->position[dim_x] - pc_visual <= 1) x[dim_x] = 0;
    else x[dim_x] = d->pc->position[dim_x] - pc_visual;

    //else pc is in at least the fourth column
    if (d->pc->position[dim_y] + pc_visual > DUNGEON_X) x[dim_y] = 0;
    else x[dim_y] = d->pc->position[dim_y] - pc_visual;

    //if pc is in the first three rows
    if (d->pc->position[dim_x] - pc_visual <= 1) y[dim_x] = 0;
    else y[dim_x] = d->pc->position[dim_x] - pc_visual;

    //if pc is in at least the fourth row
    if (d->pc->position[dim_y] + pc_visual > DUNGEON_Y) y[dim_y] = 0;
    else y[dim_y] = d->pc->position[dim_y] - pc_visual;


    pair_t foo;
    for (i = x[dim_x]; i <= x[dim_y]; i++) {
        foo[dim_x] = i;
        foo[dim_y] = y[dim_x];
        update_maps(d, pc_location, foo);

        foo[dim_y] = y[dim_y];
        update_maps(d, pc_location, foo);
    }

    for (j = y[dim_x]; j <= y[dim_y]; j++) {
        foo[dim_x] = j;
        foo[dim_y] = x[dim_x];
        update_maps(d, pc_location, foo);

        foo[dim_y] = x[dim_y];
        update_maps(d, pc_location, foo);
    }
}


//insert in bresenham's line algorithm for observing terrain
//decided to use bresenham's line algorithm because we're just checking in a straight line
int update_maps(dungeon_t *d, pair_t *f, pair_t *t) {

    bool y_negative = false, x_negative = false;
    pair_t from, to;
    int x_dir, y_dir, i1, i2, i3;

    from[dim_x] = f[dim_x];
    from[dim_y] = f[dim_y];
    to[dim_x] = t[dim_x];
    to[dim_y] = t[dim_y];

    if (to[dim_x] > from[dim_x]) {
        x_negative = true;
        x_dir = to[dim_x] - from[dim_x];
    } else x_dir = from[dim_x] - to[dim_x];


    if (to[dim_y] > from[dim_y]) {
        y_negative = true;
        y_dir = to[dim_y] - from[dim_y];
    } else y_dir = from[dim_y] - to[dim_y];


    if (y_dir > x_dir) {
        i1 = x_dir * 2;
        i2 = i1 - y_dir;
        i3 = i2 - y_dir;

        for (int i = 0; i < y_dir + 1; i++) {
            if ((mappair(from) < ter_floor) && (i != y_dir))
                return 0;

            //update visibility and terrain for from
            d->discovered[from[dim_y]][from[dim_x]] = mappair(from);
            d->visible[from[dim_y]][from[dim_x]] = 1;
            //anything else? **

            //calculating coords for next pixel
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                from[dim_x]++;
            }
        }
        return 1;
    }

    else {
        i1 = y_dir * 2;
        i2 = i1 - x_dir;
        i3 = i2 - x_dir;

        for (int i = 0; i < x_dir + 1; i++) {
            if ((mappair(from) < ter_floor) && (i != x_dir))
                return 0;

            //update visibility and terrain for from
            d->discovered[from[dim_y]][from[dim_x]] = mappair(from);
            d->visible[from[dim_y]][from[dim_x]] = 1;
            //anything else? **

            //calculating coords for next pixel
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                from[dim_y]++;
            }
        }
        return 1;
    }
}
