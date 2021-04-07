/*
  NOTES:
    - make a class to instantiate the objects we read (object_description/factory??)
      - add a method(s) to gen dynamic instances of objects
      - add a method(s) to gen dynamic instances of npc
        - npc and probably character will need to be extended to handle all new fields
      ** for everything, damage is the ONLY THING that stays as a dice, all else becomes int
      - after instantiation, place objects in the dungeon
        - for characters, extend or replace current monster generation routines
        - objects can go anywhere on the floor
        - i/o routines must be updated to render new mons and objects with colors
          - init curses color subsystem by calling start_color() when init-ing curses
          - store color attributes as pairs (background and foreground) (only need 7 bc background black)
            - if color is black, set white
            - define a color pair with int init_pair(short pair, short f, short b)
              - ex. cyan pair -->  init_pair(COLOR_CYAN. COLOR_CYAN, COLOR_BLACK)
          - render color with attron(COLOR_PAIR(index)), render the symbol, and turn the color attribute off with attroff(COLOR_PAIR(index))
            - render blue with attron(COLOR_PAIR(COLOR_BLUE))
          ** For mons with mult colors, can just use first color only
    - when a character moves over an object, render character but keep object there
    - load at least 10 objects per dungeon level
      - clean up and properly deallocate objs and mons when leaving dung level or quitting (ie have a delete method??)
    - Each artifact object may have no more than one instance in existence
      -once the PC picks up an artifact, it become ineligible for future recreation even if the PC destroys the object or abandons it on the floor
    - each unique mon may have no more than one instance in existence which becomes ineligible for generation on future dung levels only once killed m
    - select which mon or object to create by
      - uniformly selecting a rand description from your vectors of descriptions
      - if the item or mon is ineligible for gen, go to 1;
      - choose a rand int between 0 and 99 inclusive, if this num is greater than or equal to the selected mons or obj rarity, go to 1
      - gen the obj or mon and place in dungeon
      ** WE ARE NOT STACKING, TOO MUCH WORK AND ITS STUPID LOL -H
*/


/*
    CS 327: Assignment 1.08 - Loading Monsters & objects
    Sanjana Amatya, Haylee Lawrence, MyTien Kien
*/
#include <vector>
#include <stdlib.h>
#include <string.h>

//#include "npc.h"
#include "descriptions.h"
//#include "dungeon.h"
#include "assignment_108.h"
#include "utils.h"
#include "character.h"
#include "move.h"
#include "path.h"
#include "event.h"
#include "pc.h"
#include "dice.h"

// obj class moved to header file -h

// void object_delete(object *o) {
//   delete o; // IN DUNGEON.H DEFINE OBJXY AND OBJPAIR TO BE USED IN DUNGEON.CPP EMPTY DUNGEON
// }


char object::obj_symbol() {
    return object_symbol[type];
}

// make a class to instantiate the objects we read (object_description/factory??)
void gen_dynamic_obj(dungeon *d, object *o){
  // std::vector<object_description> obj_desc;
  bool done = false;
  object *obj = new object(*o);
  int idx;
  
  while (!done) {
    idx = rand() % (d->object_descriptions.size());
    uint32_t r = rand() % 100;

    if(d->object_descriptions[idx].get_artifact() && !(d->object_descriptions[idx].get_isused()) && r < d->object_descriptions[idx].get_rarity()) { //unique in use is in descriptions.h
      d->object_descriptions[idx].art_isused = true;
      //   - choose a rand int between 0 and 99 inclusive, if this num is greater than or equal to the selected mons or obj rarity, go to 1
      done = true;
    }
    else if (r < d->object_descriptions[idx].get_rarity() && !(d->object_descriptions[idx].get_artifact())) done = true;
  }

  obj->name = d->object_descriptions[idx].name;
  // set description
  obj->description = d->object_descriptions[idx].description;
  // set symbol
  obj->type = d->object_descriptions[idx].type;
  // set color
  obj->color = d->object_descriptions[idx].get_color();//[0];
  // rarity
  obj->rarity = d->object_descriptions[idx].rarity;
  // damage
  obj->damage = d->object_descriptions[idx].damage;
  // set dice speed DONT KNOW IF ROLL IS RIGHT
  obj->speed = d->object_descriptions[idx].speed.roll();
  // hp
  obj->hp = d->object_descriptions[idx].hit.roll();
  //dodge
  obj->dodge = d->object_descriptions[idx].dodge.roll();
  //defense
  obj->defense = d->object_descriptions[idx].defence.roll(); // his defense is spelt wrong (defence)
  //dodge
  obj->weight = d->object_descriptions[idx].weight.roll();
  // set value
  obj->value = d->object_descriptions[idx].value.roll();
  // set attru
  obj->attribute = d->object_descriptions[idx].attribute.roll();
  //set art
  obj->artifact = d->object_descriptions[idx].artifact;

  //place_obj(&obj);
}

// i deleted place obj bc gen_objects is doing that rn -h

// add a method(s) to gen dynamic instances of npc
// npc and probably character will need to be extended to handle all new fields
// damage is the ONLY THING that stays as a dice, all else becomes int
void gen_dynamic_mon(dungeon *d, npc *m) { // CALLED IN NPC GEN_MONSTERS
  // each unique mon may have no more than one instance in existence which becomes ineligible for generation on future dung levels only once killed m

  //   - uniformly selecting a rand description from your vectors of descriptions 0 - size 1-78 %79 +1
  int idx;
  uint32_t r;
  bool done = false;

  //   - if the item or mon is ineligible for gen, go to 1; NOT NEEDED BC UNIQUE MONS ARE REMOVED FROM VECTOR LIST
  while (!done) {
    int idx = rand() % (d->monster_descriptions.size());
    uint32_t r = rand() % 100;

    if(!(d->monster_descriptions[idx].unique_inUse) && d->monster_descriptions[idx].abilities == 'UNIQ' && r < d->monster_descriptions[idx].rarity) { //unique in use is in descriptions.h
      d->monster_descriptions[idx].unique_inUse = true;
      //   - choose a rand int between 0 and 99 inclusive, if this num is greater than or equal to the selected mons or obj rarity, go to 1
      done = true;
    }

    else if (r < d->monster_descriptions[idx].rarity && d->monster_descriptions[idx].abilities != 'UNIQ') done = true;
  }

  //   - gen the obj or mon and place in dungeon PLACED IN MAP IN GEN_MONSTERS
  // set name
  m->name = d->monster_descriptions[idx].name;
  // set description
  m->description = d->monster_descriptions[idx].description;
    // set symbol
  m->symbol = d->monster_descriptions[idx].symbol;
  // set color
  m->color = d->monster_descriptions[idx].color[1];
  // set abil
  m->abilities = d->monster_descriptions[idx].abilities;
  // set dice speed DONT KNOW IF ROLL IS RIGHT
  m->speed = d->monster_descriptions[idx].speed.roll();
  // hp
  m->hitpoints = d->monster_descriptions[idx].hitpoints.roll();
  // damage
  m->damage = d->monster_descriptions[idx].damage;
  // rarity
  m->rarity = d->monster_descriptions[idx].rarity;

}

// make unique monsters die forever
// find where monster deaths are and call this if the monster is unique
void unique_death(dungeon *d, character *m) { //CALLED IN EAT MONSTERS IN MOVE.CPP
  // find vector with same name
  int i;
  std::vector<monster_description>::iterator it;
  for (i = 0, it = d->monster_descriptions.begin(); it < d->monster_descriptions.end(); i++, it++) {
    if(d->monster_descriptions[i].name == m->name) {
      // remove the vector from the list
      d->monster_descriptions.erase(it);
      break;
    }
  }
}

void gen_objects(dungeon *d) { // CALLED IN NEW_DUNGEON IN DUNGEON.CPP
  uint32_t i;
  object *o;
  uint32_t room;
  pair_t p;
  d->max_objects = rand() % 21 + 10;
 // const static char symbol[] = "0123456789abcdef"; NOT SURE IF I SHOULD KEEP THIS

  d->num_objects = d->max_objects;

  for (i = 0; i < d->num_objects; i++) {
    o = new object;
    memset(o, 0, sizeof (*o));
    // 1.08 new code, init the unique in use
    // d->object_descriptions[i].art_isused = false; INITIALIZED IN DESCRIPTIONS.H

    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                           (d->rooms[room].position[dim_y] +
                            d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                           (d->rooms[room].position[dim_x] +
                            d->rooms[room].size[dim_x] - 1));
      } while (d->object_map[p[dim_y]][p[dim_x]] && mapxy(p[dim_x], p[dim_y]) < ter_floor);
      
      o->position[dim_y] = p[dim_y];
      o->position[dim_x] = p[dim_x];
      d->object_map[p[dim_y]][p[dim_x]] = o;
      /*    m->npc->characteristics = 0xf;*/

      // d->object_map[p[dim_y]][p[dim_x]] = o;

      // 1.08 new code
      gen_dynamic_obj(d, o);

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
