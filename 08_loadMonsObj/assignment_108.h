#ifndef ASSIGNMENT_108_H
#define ASSIGNMENT_108_H

#include "descriptions.h"
#include "dungeon.h"

class object{
}

class object_factory{
}

void gen_dynamic_obj(dungeon *d, object_description &o);
void gen_dynamic_mon(dungeon *d, npc *m);
void unique_death(dungeon *d, character *m);

#endif

