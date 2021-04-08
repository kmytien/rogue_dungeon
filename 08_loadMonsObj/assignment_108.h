#include <vector>
#include <stdlib.h>
#include <string.h>

#include "descriptions.h"
#include "dungeon.h"
#include "assignment_108.h"

/*
    CS 327: Assignment 1.08 - Loading Monsters & objects
    Sanjana Amatya, Haylee Lawrence, MyTien Kien
*/

object::object(object_description &o, pair_t p, object *next) :
          name(o.get_name()), description(o.get_description()), type(o.get_type()), color(o.get_color()),
           hp(o.get_hit().roll()), damage(o.get_damage()), dodge(o.get_dodge().roll()), defense(o.get_defence().roll()), weight(o.get_weight().roll()), speed(o.get_speed().roll()),
           attribute(o.get_attribute().roll()), value(o.get_value().roll()), artifact(o.get_artifact()), rarity(o.get_rarity()), od(o),
           seen(false) {}
{
    position[dim_x] = p[dim_x];
    position[dim_y] = p[dim_y];
}

object::~object() {
    od.destroy();
    if (next) delete next;
}

char object::obj_symbol(){
    return object_symbol[type];
}

void gen_objects(dungeon *d) { // CALLED IN NEW_DUNGEON IN DUNGEON.CPP
  uint32_t i;
  int idx;
  object *o;
  uint32_t room;
  pair_t p;
  std::vector <object_description> &desc = d->object_descriptions;

  memset(d->object_map, 0, sizeof(d->object_map));

  for (i = 0; i < d->max_objects; i++){
    do
    {
        idx = rand_range(0, desc.size() - 1);
    } while (!desc[idx].is_valid() || !desc[idx].valid_rarity());

    room = rand_range(0, d->num_rooms - 1);
    do
    {
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                           (d->rooms[room].position[dim_y] +
                            d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                           (d->rooms[room].position[dim_x] +
                            d->rooms[room].size[dim_x] - 1));
    } while (mappair(p) > ter_stairs);

    o = new object(desc[i], p, d->objmap[p[dim_y]][p[dim_x]]);

    d->object_map[p[dim_y]][p[dim_x]] = o;
  }

  d->num_objects = d->max_objects;
}

// get sybol
char object::obj_symbol()
{
    return next ? '&' : object_symbol[type];
}

// destroy all the objects in map
void destroy_objects(dungeon *d) {
    uint32_t y, x;

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (d->object_map[y][x]) {
                delete d->object_map[y][x];
                d->object_map[y][x] = 0;
            }
        }
    }
}

// make unique monsters die forever
// find where monster deaths are and call this if the monster is unique
void object_factory :: unique_death(dungeon *d, character *m) { //CALLED IN EAT MONSTERS IN MOVE.CPP
  // find vector with same name
  std::vector<monster_description>::iterator it;
  for (it = d->monster_descriptions.begin(); it < d->monster_descriptions.end(); it++) {
    if(d->monster_descriptions.name == m->name) {
      // remove the vector from the list
      d->monster_descriptions.erase(it);
      break;
    }
  }
}
  // EDIT IO
  //  - i/o routines must be updated to render new mons and objects with colors
  //    - init curses color subsystem by calling start_color() when init-ing curses
  //    - store color attributes as pairs (background and foreground) (only need 7 bc background black)
  //      - if color is black, set white
  //      - define a color pair with int init_pair(short pair, short f, short b)
  //        - ex. cyan pair -->  init_pair(COLOR_CYAN. COLOR_CYAN, COLOR_BLACK)
  //    - render color with attron(COLOR_PAIR(index)), render the symbol, and turn the color attribute off with attroff(COLOR_PAIR(index))
  //      - render blue with attron(COLOR_PAIR(COLOR_BLUE))
  //    ** For mons with mult colors, can just use first color only

  // EDIT THE PRINT CLASS when a character moves over an object, render character but keep object there
}
