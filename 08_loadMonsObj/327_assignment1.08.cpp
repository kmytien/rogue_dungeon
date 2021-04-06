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
#include "descriptions.h"
#include "dungeon.h"



//ima just do it and if we change our mind we can delete
class object {
  
public: 
  std::string name;
  std::string description;
  dice damage;
  int32_t hp, speed, abilities;
  bool is_valid;
  std::vector<uint32_t> color;
  object_description &od;

  //use damage.set(base, number, sides) to set the dice
}


// make a class to instantiate the objects we read (object_description/factory??)
class object_factory {
  
public:
  // add a method(s) to gen dynamic instances of objects
  // - select which mon or object to create by
  //   - uniformly selecting a rand description from your vectors of descriptions
  //   - if the item or mon is ineligible for gen, go to 1;
  //   - choose a rand int between 0 and 99 inclusive, if this num is greater than or equal to the selected mons or obj rarity, go to 1
  //   - gen the obj or mon and place in dungeon
  // - Each artifact object may have no more than one instance in existence
  //   -once the PC picks up an artifact, it become ineligible for future recreation even if the PC destroys the object or abandons it on the floor
  // - each unique mon may have no more than one instance in existence which becomes ineligible for generation on future dung levels only once killed m
  void gen_dynamic_obj(dungeon *d, object_description &o){
    // std::vector<object_description> obj_desc;

    int idx = rand() % d->object_descriptions.size();

    o.name = d->object_descriptions[idx].name;
    // set description
    o.description = d->object_descriptions[idx].description;
    // set symbol
    o.type = d->object_descriptions[idx].type;
    // set color
    o.color = d->object_descriptions[idx].color[1];
    // rarity
    o.rarity = d->object_descriptions[idx].rarity[1];
    // damage
    o.damage = roll(d->object_descriptions[idx].damage);
    // set dice speed DONT KNOW IF ROLL IS RIGHT
    o.speed = roll(d->object_descriptions[idx].speed);
    // hp
    o.hit = roll(d->object_descriptions[idx].hit);
    //dodge
    o.dodge = roll(d->object_descriptions[idx].dodge);
    //defense
    o.defence = roll(d->object_descriptions[idx].defence);
    //dodge
    o.weight = roll(d->object_descriptions[idx].weight);
    // set value
    o.value = roll(d->object_descriptions[idx].value);
    // set attru
    o.attrubute = roll(d->object_descriptions[idx].attrubute);
    //set art
    o.art = d->object_description[idx].art;
  }
  
  // add a method(s) to gen dynamic instances of npc
  // npc and probably character will need to be extended to handle all new fields
  // damage is the ONLY THING that stays as a dice, all else becomes int 
  void gen_dynamic_mon(dungeon *d, npc *m) { // IM GONNA EDIT GEN_MONSTERS SO THAT IT EDITS THE MONSTER THAT WAS CREATED BASED OFF OF DESCRIPTIONS
    // each unique mon may have no more than one instance in existence which becomes ineligible for generation on future dung levels only once killed m

    //   - uniformly selecting a rand description from your vectors of descriptions 0 - size 1-78 %79 +1
    int idx = rand() % d->monster_descriptions.size();
    uint32_t r = rand() % 100;
    
    //   - if the item or mon is ineligible for gen, go to 1; NOT NEEDED BC UNIQUE MONS ARE REMOVED FROM VECTOR LIST
    if(!(d->monster_descriptions.unique_inUse)) idx = 1;
    //   - choose a rand int between 0 and 99 inclusive, if this num is greater than or equal to the selected mons or obj rarity, go to 1
    else if (r >= d->monster_descriptions[idx].rarity) idx = 1;
    
    //   - gen the obj or mon and place in dungeon PLACED IN MAP IN GEN_MONSTERS
    // set name
    m.name = d->monster_descriptions[idx].name;
    // set description
    m.description = d->monster_descriptions[idx].description;
    // set symbol
    m.symbol = d->monster_descriptions[idx].symbol;
    // set color
    m.color = d->monster_descriptions[idx].color[1];
    // set abil
    m.abilities = d->monster_descriptions[idx].abilities;
    // set dice speed DONT KNOW IF ROLL IS RIGHT
    m.speed = roll(d->monster_descriptions[idx].speed);
    // hp
    m.hitpoints = roll(d->monster_descriptions[idx].hitpoints);
    // damage
    m.damage = d->monster_descriptions[idx].damage;
    // rarity
    m.rarity = d->monster_descriptions[idx].rarity;

  }

  // make unique monsters die forever
  // find where monster deaths are and call this if the monster is unique
  void unique_death(dungeon *d, character *m) {
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
  
  // after instantiation, place objects in the dungeon
  //  - objects can go anywhere on the floor
  place_obj(object obj){
    int x = 0, y = 0;
    //spawn objects anywhere where there is floor (mapxy(x,y) >= ter_floor)
    //can be walked over - should we make an object map for this then??

    //makes sure coordinated at x and y are floor
    while (mapxy(x, y) < ter_floor) {
      x = (rand() % DUNGEON_X) + 1;
      y = (rand() % DUNGEON_Y) + 1;
    }

    d->object_map[y][x] = obj;
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
