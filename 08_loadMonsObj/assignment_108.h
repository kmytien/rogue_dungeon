#ifndef ASSIGNMENT_108_H
#define ASSIGNMENT_108_H

#include "descriptions.h"
#include "dungeon.h"

class object {
private:
  std::string name, description;
  object_type_t type;
  dice damage;
  uint32_t hp, speed, attribute, dodge, defense, weight, attribute value, rarity;
  bool art_isused, artifact;
  uint32_t color;
  pair_t position;
  object next;
  object_description od;
  bool seen;

  inline bool is_valid() {
    return (!(artifact) || ((artifact) && !art_isused))
  }
  inline bool valid_rarity() { return rarity > (unsigned) (rand() % 100); }
public:
  object(object_description &o, pair_t p, object *next);

  ~object();

  char obj_symbol();
}

// void object_delete(object *o);
void gen_objects(dungeon *d);
void destroy_objects(dungeon *d);

#endif
