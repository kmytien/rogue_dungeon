#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"
# include "character.h"
# include "dungeon.h"

class pc : public character {
 public:
  ~pc() {}

  object *equipment[12]; // [WEAPON][OFFHAND][RANGED][ARMOR][HELMET][CLOAK][GLOVES][BOOTS][AMULET][LIGHT][RING][RING]
  object *inventory[10];

  terrain_type known_terrain[DUNGEON_Y][DUNGEON_X];
  uint8_t visible[DUNGEON_Y][DUNGEON_X];
  uint32_t pc_wear_item(uint32_t empty_slot);
  uint32_t pc_remove_equipment(uint32_t empty_slot);
  uint32_t pc_drop_equipment(uint32_t empty_slot);
  uint32_t pc_permanent_itemRemoval(uint32_t empty_slot);
};

void pc_delete(pc *pc);
uint32_t pc_is_alive(dungeon *d);
void config_pc(dungeon *d);
uint32_t pc_next_pos(dungeon *d, pair_t dir);
void place_pc(dungeon *d);
uint32_t pc_in_room(dungeon *d, uint32_t room);
void pc_learn_terrain(pc *p, pair_t pos, terrain_type ter);
terrain_type pc_learned_terrain(pc *p, int16_t y, int16_t x);
void pc_init_known_terrain(pc *p);
void pc_observe_terrain(pc *p, dungeon *d);
int32_t is_illuminated(pc *p, int16_t y, int16_t x);
void pc_reset_visibility(pc *p);

int32_t pc_take(dungeon *d, object *o);

void init_slots(dungeon *d);
int32_t pc_equip(dungeon *d, object *o);
void pc_stat_refresh(dungeon *d);

#endif
