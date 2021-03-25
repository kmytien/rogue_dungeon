#include <stdlib.h>

#include "character.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "dungeon.h"
#include "assignment_106.h"

  
//initialization of both maps (terrain and visibility for fog of war)
void init_maps(dungeon_t *d) {
    int y, x;

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->discovered[y][x] = ter_wall; 
            d->visible[y][x] = 0; 
        }
    }
}


//radius of whats visible to pc
void update_sight(dungeon_t *d) {

    int i, j, pc_visual = 3;
    pair_t x, y, pc_location; //storing starting (dim_x) and end point (dim_y) for x/y ranges

    pc_location[dim_x] = d->pc.position[dim_x];
    pc_location[dim_y] = d->pc.position[dim_y];

		//resets visibility
		for (int k = 0; k < DUNGEON_Y; k++) {
			for (int m = 0; m < DUNGEON_X; m++) {
					d->visible[k][m] = 0;
			}
		}

		//the following tells us the range for x and y (of the illuminated box)
		//if pc is in the first 3 columns
    if (pc_location[dim_x] - pc_visual < 0) x[dim_x] = 0;
		else x[dim_x] = pc_location[dim_x] - pc_visual;

		//else pc is in at least the fourth column
		if (pc_location[dim_x] + pc_visual > DUNGEON_X - 1) x[dim_y] = DUNGEON_X - 1;
		else x[dim_y] = pc_location[dim_x] + pc_visual;

		//if pc is in the first three rows
		if (pc_location[dim_y] - pc_visual < 0) y[dim_x] = 0;
		else y[dim_x] = pc_location[dim_y] - pc_visual;

		//if pc is in at least the fourth row
		if (pc_location[dim_y] + pc_visual > DUNGEON_Y - 1) y[dim_y] = DUNGEON_Y - 1;
		else y[dim_y] = pc_location[dim_y] + pc_visual;

		//goes through the bresenheim's line algorithm and updates terrain/visibility as necessary
    pair_t foo;
    for (i = x[dim_x]; i <= x[dim_y]; i++) {
        foo[dim_x] = i;
        foo[dim_y] = y[dim_x];
        update_maps(d, pc_location, foo);

        foo[dim_y] = y[dim_y];
        update_maps(d, pc_location, foo);
    }

    for (j = y[dim_x]; j <= y[dim_y]; j++) {
        foo[dim_y] = j;
        foo[dim_x] = x[dim_x];
        update_maps(d, pc_location, foo);

        foo[dim_x] = x[dim_y];
        update_maps(d, pc_location, foo);
    }
}


//used sheaffer's suggestion to use bresenheim's line algorithm 
//(we just copied/pasted our line algorithm from 1.04 and modified it)
int update_maps(dungeon_t *d, pair_t f, pair_t t) {

    bool y_negative = false, x_negative = false;
    pair_t from, to;
    int x_dir, y_dir, i1, i2, i3;

    from[dim_x] = f[dim_x];
    from[dim_y] = f[dim_y];
    to[dim_x] = t[dim_x];
    to[dim_y] = t[dim_y];

    if (to[dim_x] < from[dim_x]) {
        x_negative = true;
        x_dir = from[dim_x] - to[dim_x];
    } else x_dir = to[dim_x] - from[dim_x];

    if (to[dim_y] < from[dim_y]) {
        y_negative = true;
        y_dir = from[dim_y] - to[dim_y];
    } else y_dir = to[dim_y] - from[dim_y];


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
            
						if (y_negative) from[dim_y]--;
            else from[dim_y]++;
                
            //calculating coords for next pixel
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                
                if (x_negative) from[dim_x]--;
                else from[dim_x]++;
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

						if (x_negative) from[dim_x]--;
            else from[dim_x]++;
                
            //calculating coords for next pixel
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                if (y_negative) from[dim_y]--;
                else from[dim_y]++;
            }
        }
        return 1;
    }
}
