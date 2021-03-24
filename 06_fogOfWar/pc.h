#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"

// typedef struct dungeon dungeon_t;

// typedef struct pc {
// } pc_t;

class pc_t: public character{
    public:
    
};

    void pc_delete(pc_t *pc);
    uint32_t pc_is_alive(dungeon *d);
    void config_pc(dungeon *d);
    uint32_t pc_next_pos(dungeon *d, pair_t dir);
    void place_pc(dungeon *d);
    uint32_t pc_in_room(dungeon *d, uint32_t room);

#endif
