#ifndef DESCRIPTIONS_H
# define DESCRIPTIONS_H

# include <stdint.h>
# include <vector>
# include <string>

# include "dice.h"
# include "npc.h"

class dungeon;

uint32_t parse_descriptions(dungeon *d);

uint32_t print_descriptions(dungeon *d);

uint32_t destroy_descriptions(dungeon *d);

typedef enum object_type {
    objtype_no_type,
    objtype_WEAPON,
    objtype_OFFHAND,
    objtype_RANGED,
    objtype_LIGHT,
    objtype_ARMOR,
    objtype_HELMET,
    objtype_CLOAK,
    objtype_GLOVES,
    objtype_BOOTS,
    objtype_AMULET,
    objtype_RING,
    objtype_SCROLL,
    objtype_BOOK,
    objtype_FLASK,
    objtype_GOLD,
    objtype_AMMUNITION,
    objtype_FOOD,
    objtype_WAND,
    objtype_CONTAINER
} object_type_t;

extern const char object_symbol[];

class npc;

class monster_description {
private:
    std::string name, description;
    char symbol;
    std::vector <uint32_t> color;
    uint32_t abilities;
    dice speed, hitpoints, damage;
    uint32_t rarity;
    uint32_t alive, dead; // dead if unique is important

    inline bool generating() {
        return (((abilities & NPC_UNIQ) && !alive && !dead) || !(abilities & NPC_UNIQ)); //i found the uniq stuff used in desc so this should work? i think abilities == 'UNIQ' might also work???? -h
    }

    inline bool rr_roll() {
        return rarity > (uint32_t) (rand() % 100); //
    }

public:
    monster_description() : name(), description(), symbol(0), color(0), // pretty sure we can use new??? with this??
                            abilities(0), speed(), hitpoints(), damage(),
                            rarity(0), alive(0), dead(0)
    {
    }

    void set(const std::string &name,
             const std::string &description,
             const char symbol,
             const std::vector <uint32_t> &color,
             const dice &speed,
             const uint32_t abilities,
             const dice &hitpoints,
             const dice &damage,
             const uint32_t rarity);

    std::ostream &print(std::ostream &o);

    char get_symbol() {
    	return symbol;
    }

    inline void birth() {
        alive++;
    }

    inline void die() {
        dead++;
        alive--;
    }

    inline void destroy() {
        alive--;
    }

    static npc *gen_dyn_monster(dungeon *d);

    friend npc;
};

class object_description {
private:
    std::string name, description;
    object_type_t type;
    uint32_t color;
    dice hit, damage, dodge, defense, weight, speed, attribute, value;
    bool artifact;
    uint32_t rarity;
    bool art_made, art_found;
public:
    object_description() : name(), description(), type(objtype_no_type),
                           color(0), hit(), damage(),
                           dodge(), defense(), weight(),
                           speed(), attribute(), value(),
                           artifact(false), rarity(0), art_made(false),
                           art_found(false)
    {
    }

    inline bool generating() {
        return !artifact || (artifact && !art_made && !art_found);
    }

    inline bool rr_roll() {
        return rarity > (unsigned) (rand() % 100);
    }

    void set(const std::string &name,
             const std::string &description,
             const object_type_t type,
             const uint32_t color,
             const dice &hit,
             const dice &damage,
             const dice &dodge,
             const dice &defense,
             const dice &weight,
             const dice &speed,
             const dice &attrubute,
             const dice &value,
             const bool artifact,
             const uint32_t rarity);

    std::ostream &print(std::ostream &o);

    /* Need all these accessors because otherwise there is a *
     * circular dependancy that is difficult to get around.  */
    inline const std::string &get_name() const
    { return name; }

    inline const std::string &get_description() const
    { return description; }

    inline const object_type_t get_type() const
    { return type; }

    inline const uint32_t get_color() const
    { return color; }

    inline const dice &get_hit() const
    { return hit; }

    inline const dice &get_damage() const
    { return damage; }

    inline const dice &get_dodge() const
    { return dodge; }

    inline const dice &get_defense() const
    { return defense; }

    inline const dice &get_weight() const
    { return weight; }

    inline const dice &get_speed() const
    { return speed; }

    inline const dice &get_attribute() const { return attribute; }

    inline const dice &get_value() const { return value; }

};

std::ostream &operator<<(std::ostream &o, monster_description &m);
std::ostream &operator<<(std::ostream &o, object_description &od);

#endif
