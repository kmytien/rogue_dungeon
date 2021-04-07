#ifndef ASSIGNMENT_108_H
#define ASSIGNMENT_108_H

#include "descriptions.h"
#include "dungeon.h"

class object {
public:
  object() : name(), description(), type(objtype_no_type), color(0),
             hp(0), damage(), dodge(0), defense(0), weight(0), speed(p),
             attribute(0), value(0), artifact(false), rarity(0)
  std::string name, description;
  object_type_t type;
  dice damage;
  uint32_t hp, speed, attribute, dodge, defense, weight, attribute value, rarity;
  bool art_isused, artifact;
  uint32_t color;
  object_description &od;
  
  char obj_symbol();
}

class object_factory{
public:
  void gen_dynamic_obj(dungeon *d, object_description *o);
  void gen_dynamic_mon(dungeon *d, npc *m);
  void unique_death(dungeon *d, character *m);
}

// void object_delete(object *o);
void gen_objects(dungeon *d);

#endif
