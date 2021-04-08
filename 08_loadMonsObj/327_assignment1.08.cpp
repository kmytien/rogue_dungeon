#include <vector>
#include <cstring>

#include "dungeon.h"
#include "utils.h"
#include "assignment_108.h"

object::object(object_description &o, pair_t p) :
        name(o.get_name()),
        description(o.get_description()),
        type(o.get_type()),
        color(o.get_color()),
        damage(o.get_damage()),
        hit(o.get_hit().roll()),
        dodge(o.get_dodge().roll()),
        defense(o.get_defense().roll()),
        weight(o.get_weight().roll()),
        speed(o.get_speed().roll()),
        attribute(o.get_attribute().roll()),
        value(o.get_value().roll()),
        shown(false)
{
    position[dim_x] = p[dim_x];
    position[dim_y] = p[dim_y];
}

// METHOD TO GEN THE DYNAMIC OBJECTS, SIMILAR TO GEN_MONSTERS IN NPC
void gen_dyn_object(dungeon *d) {
		std::vector <object_description> &foo = d->obj_desc;
    object *o;
    pair_t p;
    int j;
		uint32_t i, room;
		
		memset(d->object_map, 0, sizeof(d->object_map));
		
    for (i = 0; i < 15; i++) {
		  
		  j = rand_range(0, foo.size() - 1);
		  room = rand_range(0, d->num_rooms - 1);
		  
		  //copied this from shaeffer's code (in dungeon)
		  do {
		      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
		                            (d->rooms[room].position[dim_y] +
		                             d->rooms[room].size[dim_y] - 1));
		      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
		                            (d->rooms[room].position[dim_x] +
		                             d->rooms[room].size[dim_x] - 1));
		  } while (mappair(p) > ter_stairs);

		  o = new object(foo[j], p);
		  d->object_map[p[dim_y]][p[dim_x]] = o;
		}
		
		d->num_objects = 15;
}

char object::obj_symbol() {
    return object_symbol[type];
}

// SHEAFFER SAID WE NEEDED THIS IN CLASS IDK IF WHERE WE SHOULD USE IT -H idk either but ill take care of it 
void destroy_objects(dungeon *d) {
    uint32_t y, x;

    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (d->object_map[y][x]) {
                delete d->object_map[y][x]; // idk if this works but i found this done in description ik?
                d->object_map[y][x] = 0;
            }
        }
    }
}
