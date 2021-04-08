#ifndef OBJECT_H
# define OBJECT_H

# include <string>

# include "descriptions.h"
# include "dims.h"

class object {
public:
    const std::string &name;
    const std::string &description;
    object_type_t type;
    uint32_t color;
    pair_t position;
    const dice &damage;
    int32_t hit, dodge, defense, weight, speed, attribute, value;
    bool shown;
    object_description &od;

    object(object_description &o, pair_t p);
    ~object();

    char obj_symbol();

    void revealed() { 
    	shown = true; 
    }
};

void gen_dyn_object(dungeon *d);
void destroy_objects(dungeon *d);

#endif
