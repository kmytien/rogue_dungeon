#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"
#include "io.h"
#include "npc.h"
#include "dice.h"

void do_combat(dungeon *d, character *atk, character *def)
{
    uint32_t dam, i;
    const char *organs[] = {
      "liver",                   /*  0 */
      "pancreas",                /*  1 */
      "heart",                   /*  2 */
      "eye",                     /*  3 */
      "arm",                     /*  4 */
      "leg",                     /*  5 */
      "intestines",              /*  6 */
      "gall bladder",            /*  7 */
      "lungs",                   /*  8 */
      "hand",                    /*  9 */
      "foot",                    /* 10 */
      "spinal cord",             /* 11 */
      "pituitary gland",         /* 12 */
      "thyroid",                 /* 13 */
      "tongue",                  /* 14 */
      "bladder",                 /* 15 */
      "diaphram",                /* 16 */
      "stomach",                 /* 17 */
      "pharynx",                 /* 18 */
      "esophagus",               /* 19 */
      "trachea",                 /* 20 */
      "urethra",                 /* 21 */
      "spleen",                  /* 22 */
      "ganglia",                 /* 23 */
      "ear",                     /* 24 */
      "subcutaneous tissue"      /* 25 */
      "cerebellum",              /* 26 */ /* Brain parts begin here */
      "hippocampus",             /* 27 */
      "frontal lobe",            /* 28 */
      "brain",                   /* 29 */
    };
    if (def->alive)
    {
      //if attacker not pc then give diff message
      if (atk != d->PC) {
        dam = atk->damage->roll();
        io_queue_message("The %s %s your %s for %d damage.",atk->name, "stabs", 
                             organs[rand() % (sizeof(organs) / sizeof(organs[0]))], dam);
      }
      // since attacker is the pc, give pc dam message
      else {
        // get orig damage
        dam = atk->damage->roll();
        
        // get added damage bonuses for equipment
        for (i = 0; i < 12; i++) {
          if (d->PC->equipment[i] != NULL) {
            dam += d->PC->equipment[i]->roll_dice();
          }
        }
      
        io_queue_message("You hit the %s for %d damage.", def->name, dam);
        //if(atk == d->PC) dam = 1000;  // TESTING PURPOSES
      }
	
      // if the hit kills def (old combat code)
      if (dam >= def->hp) {
        // if pc isnt killed (old combat code)
        if (atk != d->PC) {
           io_queue_message("You die.");
            io_queue_message("As %s%s eats your %s,", is_unique(atk) ? "" : "the ",
                             atk->name, organs[rand() % (sizeof(organs) / sizeof(organs[0]))]);
            io_queue_message("   ...you wonder if there is an afterlife.");
            /* Queue an empty message, otherwise the game will not pause for *
             * player to see above.                                          */
             io_queue_message("");
        } else {
          io_queue_message("%s%s dies.", is_unique(def) ? "" : "The ", def->name);
        }
        def->hp = 0;
        def->alive = 0;
        character_increment_dkills(atk);
        character_increment_ikills(atk, (character_get_dkills(def) +
                                         character_get_ikills(def)));
        if (def != d->PC) {
          d->num_monsters--;
        }
          charpair(def->position) = NULL;
      } 
      // if the hit doesnt kill then just subtract it from defs hp
      else def->hp -= dam;
    }
}

void move_character(dungeon *d, character *c, pair_t next)
{
  //int can_see_atk, can_see_def;   
  pair_t cell;
  uint32_t valid_cell;
  // WHATS USED TO MAKE EASY SHIFTING, RECOMMENDED BY SCHAEFFER AND PIAZZA(?)
  pair_t p[9] = { {-1, -1},{-1, 0},{-1, 1},{0, -1},{0, 0},{0, 1},{1, -1},{1, 0},{1, 1}, };
  uint32_t s, i;  
  
  // EQUIP CALL CODE
  // if there is an object in the new position & c is the pc
  if (d->objmap[next[dim_y]][next[dim_x]] != NULL && c == d->PC) {
    // equip the item
    //changed equip to take method
    if (!pc_take(d, d->objmap[next[dim_y]][next[dim_x]])) {
      // remove item from the object map
      d->objmap[next[dim_y]][next[dim_x]] = NULL;
      pc_stat_refresh(d);
      // if it is an artifact, set as not again spawnable
      //d->objmap[next[dim_y]][next[dim_x]].has_been_seen();
    }
  }
  
  //if there is a character in the new position
  if (charpair(next) &&
      ((next[dim_y] != c->position[dim_y]) ||
       (next[dim_x] != c->position[dim_x]))) {
    // if pc is def or atk do combat
    if ((charpair(next) == d->PC) || c == d->PC) do_combat(d, c, charpair(next));
    // else move mons out of the way
    else {
      // for random num till cell is valid or for all the p pairs
      for (s = rand() % 9, valid_cell = i = 0; i < 9 && !valid_cell; i++) {
        // cell is next + adjusted val (p[s%9] was on Piazza)
        cell[dim_y] = next[dim_y] + p[s%9][dim_y];
        cell[dim_x] = next[dim_x] + p[s%9][dim_x];
        // if can pass wall or if cell has atk or is empty then its valid
        if ((((npc *) charpair(next))->characteristics & NPC_PASS_WALL) && (!charpair(cell) || (charpair(cell) == c))) {
          valid_cell = 1;
        }
        // else if theres no char and its not wall or if atk is there then its valid
        else if ((!charpair(cell) && (mappair(cell) >= ter_floor)) || (charpair(cell) == c)) valid_cell = 1;
      }
      // set positions
      charpair(c->position) = NULL;
      charpair(cell) = charpair(next);
      charpair(next) = c;
      charpair(cell)->position[dim_y] = cell[dim_y];
      charpair(cell)->position[dim_x] = cell[dim_x];
      c->position[dim_y] = next[dim_y];
      c->position[dim_x] = next[dim_x];
    }
  } 
  else {
    /* No character in new position. */

    d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
    c->position[dim_y] = next[dim_y];
    c->position[dim_x] = next[dim_x];
    d->character_map[c->position[dim_y]][c->position[dim_x]] = c;
  }

  if (c == d->PC) {
    pc_reset_visibility(d->PC);
    pc_observe_terrain(d->PC, d);
  }
}

void do_moves(dungeon *d)
{
  pair_t next;
  character *c;
  event *e;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    /* The PC always goes first one a tie, so we don't use new_event().  *
     * We generate one manually so that we can set the PC sequence       *
     * number to zero.                                                   */
    e = (event *) malloc(sizeof (*e));
    e->type = event_character_turn;
    /* Hack: New dungeons are marked.  Unmark and ensure PC goes at d->time, *
     * otherwise, monsters get a turn before the PC.                         */
    if (d->is_new) {
      d->is_new = 0;
      e->time = d->time;
    } else {
      e->time = d->time + (1000 / d->PC->speed);
    }
    e->sequence = 0;
    e->c = d->PC;
    heap_insert(&d->events, e);
  }

  while (pc_is_alive(d) &&
         (e = (event *) heap_remove_min(&d->events)) &&
         ((e->type != event_character_turn) || (e->c != d->PC))) {
    d->time = e->time;
    if (e->type == event_character_turn) {
      c = e->c;
    }
    if (!c->alive) {
      if (d->character_map[c->position[dim_y]][c->position[dim_x]] == c) {
        d->character_map[c->position[dim_y]][c->position[dim_x]] = NULL;
      }
      if (c != d->PC) {
        event_delete(e);
      }
      continue;
    }

    npc_next_pos(d, (npc *) c, next);
    move_character(d, (npc *) c, next);

    heap_insert(&d->events, update_event(d, e, 1000 / c->speed));
  }

  io_display(d);
  if (pc_is_alive(d) && e->c == d->PC) {
    c = e->c;
    d->time = e->time;
    /* Kind of kludgey, but because the PC is never in the queue when   *
     * we are outside of this function, the PC event has to get deleted *
     * and recreated every time we leave and re-enter this function.    */
    e->c = NULL;
    event_delete(e);
    io_handle_input(d);
  }
}

void dir_nearest_wall(dungeon *d, character *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (c->position[dim_x] != 1 && c->position[dim_x] != DUNGEON_X - 2) {
    dir[dim_x] = (c->position[dim_x] > DUNGEON_X - c->position[dim_x] ? 1 : -1);
  }
  if (c->position[dim_y] != 1 && c->position[dim_y] != DUNGEON_Y - 2) {
    dir[dim_y] = (c->position[dim_y] > DUNGEON_Y - c->position[dim_y] ? 1 : -1);
  }
}

uint32_t against_wall(dungeon *d, character *c)
{
  return ((mapxy(c->position[dim_x] - 1,
                 c->position[dim_y]    ) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x] + 1,
                 c->position[dim_y]    ) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x]    ,
                 c->position[dim_y] - 1) == ter_wall_immutable) ||
          (mapxy(c->position[dim_x]    ,
                 c->position[dim_y] + 1) == ter_wall_immutable));
}

uint32_t in_corner(dungeon *d, character *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(c->position[dim_x] - 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x] + 1,
                          c->position[dim_y]    ) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] - 1) == ter_wall_immutable);
  num_immutable += (mapxy(c->position[dim_x]    ,
                          c->position[dim_y] + 1) == ter_wall_immutable);

  return num_immutable > 1;
}

static void new_dungeon_level(dungeon *d, uint32_t dir)
{
  /* Eventually up and down will be independantly meaningful. *
   * For now, simply generate a new dungeon.                  */

  switch (dir) {
  case '<':
  case '>':
    new_dungeon(d);
    break;
  default:
    break;
  }
}


uint32_t move_pc(dungeon *d, uint32_t dir)
{
  pair_t next;
  uint32_t was_stairs = 0;
  const char *wallmsg[] = {
    "There's a wall in the way.",
    "BUMP!",
    "Ouch!",
    "You stub your toe.",
    "You can't go that way.",
    "You admire the engravings.",
    "Are you drunk?"
  };

  next[dim_y] = d->PC->position[dim_y];
  next[dim_x] = d->PC->position[dim_x];


  switch (dir) {
  case 1:
  case 2:
  case 3:
    next[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    next[dim_y]--;
    break;
  }
  switch (dir) {
  case 1:
  case 4:
  case 7:
    next[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    next[dim_x]++;
    break;
  case '<':
    if (mappair(d->PC->position) == ter_stairs_up) {
      was_stairs = 1;
      new_dungeon_level(d, '<');
    }
    break;
  case '>':
    if (mappair(d->PC->position) == ter_stairs_down) {
      was_stairs = 1;
      new_dungeon_level(d, '>');
    }
    break;
  }

  if (was_stairs) {
    return 0;
  }

  if ((dir != '>') && (dir != '<') && (mappair(next) >= ter_floor)) {
    move_character(d, d->PC, next);
    dijkstra(d);
    dijkstra_tunnel(d);

    return 0;
  } else if (mappair(next) < ter_floor) {
    io_queue_message(wallmsg[rand() % (sizeof (wallmsg) /
                                       sizeof (wallmsg[0]))]);
    io_display(d);
  }

  return 1;
}
