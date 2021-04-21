#include <vector>
#include <cstring>

#include "object.h"
#include "dungeon.h"
#include "utils.h"
#include "character.h"
#include "pc.h"

object::object(object_description &o, pair_t p, object *next) :
  name(o.get_name()),
  description(o.get_description()),
  type(o.get_type()),
  color(o.get_color()),
  damage(o.get_damage()),
  hit(o.get_hit().roll()),
  dodge(o.get_dodge().roll()),
  defence(o.get_defence().roll()),
  weight(o.get_weight().roll()),
  speed(o.get_speed().roll()),
  attribute(o.get_attribute().roll()),
  value(o.get_value().roll()),
  seen(false),
  next(next),
  od(o)
{
  position[dim_x] = p[dim_x];
  position[dim_y] = p[dim_y];

  od.generate();
}

object::~object()
{
  od.destroy();
  if (next) {
    delete next;
  }
}

//NEW CODE
void gen_potions(dungeon *d)
{
  object *o;
  uint32_t room;
  pair_t p;
  std::vector<object_description> &v = d->object_descriptions;
  int i;

  do {
    i = rand_range(0, v.size() - 1);
  } while ((!v[i].can_be_generated() || !v[i].pass_rarity_roll()) && v[i].get_type() == objtype_POTION);
  
  room = rand_range(0, d->num_rooms - 1);
  do {
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                          (d->rooms[room].position[dim_x] +
                           d->rooms[room].size[dim_x] - 1));
  } while (mappair(p) > ter_stairs);

  o = new object(v[i], p, d->objmap[p[dim_y]][p[dim_x]]);

  d->objmap[p[dim_y]][p[dim_x]] = o;
  
}

// NEW CODE
void drink_potion(dungeon *d, pair_t next) {
  int h;
  
  if(!(d->pc_health)) h = 1000;
  else h = d->pc_health;
  
  std::string name;
  name = d->objmap[next[dim_y]][next[dim_x]]->get_name();

  // increase health based off of hit from potion
  if (name == "godly health potion") {
    if(!(d->pc_health)) d->PC->hp = 1000;
    if ((d->pc_health)) d->PC->hp = d->pc_health;
  }
  else if (name == "spicy health potion") d->PC->hp += h/50;
  else if (name == "good sized health potion") d->PC->hp += h/75;
  else d->PC->hp += h/25;
  
  // cant go over max
  if(!(d->pc_health) && d->PC->hp > 1000) d->PC->hp = 1000;
   if ((d->pc_health) && d->PC->hp > d->pc_health) d->PC->hp = d->pc_health;
}

void gen_object(dungeon *d)
{
  object *o;
  uint32_t room;
  pair_t p;
  std::vector<object_description> &v = d->object_descriptions;
  int i;

  do {
    i = rand_range(0, v.size() - 1);
  } while (!v[i].can_be_generated() || !v[i].pass_rarity_roll());
  
  room = rand_range(0, d->num_rooms - 1);
  do {
    p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                          (d->rooms[room].position[dim_y] +
                           d->rooms[room].size[dim_y] - 1));
    p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                          (d->rooms[room].position[dim_x] +
                           d->rooms[room].size[dim_x] - 1));
  } while (mappair(p) > ter_stairs);

  o = new object(v[i], p, d->objmap[p[dim_y]][p[dim_x]]);

  d->objmap[p[dim_y]][p[dim_x]] = o;
  
}

void gen_objects(dungeon *d)
{
  uint32_t i, p_num;
  p_num = (rand()%6) + 3;

  memset(d->objmap, 0, sizeof (d->objmap));

  for (i = 0; i < d->max_objects; i++) {
    gen_object(d);
  }
  
  for (i = 0; i < p_num; i++) {
    gen_potions(d);
  }

  d->num_objects = d->max_objects + p_num;
}

char object::get_symbol()
{
  return next ? '&' : object_symbol[type];
}

uint32_t object::get_color()
{
  return color;
}

const char *object::get_name()
{
  return name.c_str();
}

int32_t object::get_speed()
{
  return speed;
}

int32_t object::roll_dice()
{
  return damage.roll();
}

void destroy_objects(dungeon *d)
{
  uint32_t y, x;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->objmap[y][x]) {
        delete d->objmap[y][x];
        d->objmap[y][x] = 0;
      }
    }
  }
}

int32_t object::get_type()
{
  return type;
}

uint32_t object::is_equipable()
{
  return type >= objtype_WEAPON && type <= objtype_RING; 
}

uint32_t object::is_removable()
{
  return 1;
}

uint32_t object::is_dropable()
{
  return 1;
}

uint32_t object::is_destructable()
{
  return 1;
}

int32_t object::get_eq_slot_index()
{
  if (type < objtype_WEAPON ||
      type > objtype_RING) {
    return -1;
  }

  return type - 1;
}

void object::to_pile(dungeon *d, pair_t location)
{
  next = (object *) d->objmap[location[dim_y]][location[dim_x]];
  d->objmap[location[dim_y]][location[dim_x]] = this;
}
