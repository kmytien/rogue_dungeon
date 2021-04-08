#include <vector>
#include <cstring>

#include "assignment_108.h"
#include "dungeon.h"
#include "utils.h"

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
    if (next)
    {
        delete next;
    }
}

void gen_object(dungeon *d)
{
    object *o;
    uint32_t room;
    pair_t p;
    std::vector <object_description> &v = d->obj_desc;
    int j;
		uint32_t i;
		
		memset(d->object_map, 0, sizeof(d->object_map));
		
    for (i = 0; i < d->max_objects; i++) {
		  do
		  {
		      j = rand_range(0, v.size() - 1);
		  } while (!v[j].generating() || !v[j].rr_roll());

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

		  o = new object(v[j], p, d->object_map[p[dim_y]][p[dim_x]]);

		  d->object_map[p[dim_y]][p[dim_x]] = o;
		}
		
		d->num_objects = d->max_objects;
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

    for (y = 0; y < DUNGEON_Y; y++)
    {
        for (x = 0; x < DUNGEON_X; x++)
        {
            if (d->object_map[y][x])
            {
                delete d->object_map[y][x];
                d->object_map[y][x] = 0;
            }
        }
    }
}

int32_t object::get_type()
{
    return type;
}
