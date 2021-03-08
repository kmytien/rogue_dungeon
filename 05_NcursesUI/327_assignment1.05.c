#include <stdio.h>
#include <ncurses.h>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "npc.h"


/**
  NOTES:
  - Make staircases functional
    - when a player goes up or down stairs generate a new dungeon and new monsters (up <, down >) WE SHOULD DO NON PERSISTANT/YOU CANNOT GO BACK TO THE SAME LEVEL
      - PC gets put in a random spot on the floor
  - Set commands NEVER NEED TO PRESS ENTER, A PRESSED KEY ACTIVATES COMMAND IMMEDIATELY, PRINT MESSAGES IF A COMMAND IS NOT POSSIBLE
    - <, > go up (<) or down (>) stairs if on a stair block ** NPCS CANNOT USE STAIRS
    - 7, y move PC 1 cell to the upper left
    - 8, k move PC 1 cell up
    - 9, u move PC 1 cell to upper right
    - 6, l move PC one cell to the right
    - 3, n move PC one cell to lower right
    - 2, j move PC one cell down
    - 1, b move PC one cell to the lower right
    - 4, h move PC one cell to the left
    - 5, [SPACE], . Rest for turn, NPCs still move
    - m display a list of monsters in the dungeon, with their symbol and position relative to the PC (e.g.: “c, 2 north and 14 west”).
    - [UP ARROW] when displaying monster list, if entire list does not fit in screen and not currently at top of list, scroll list up
    - [DOWN ARROW] when displaying monster list, if entire list does not fit in screen and not currently at bottom of list, scroll list down
    - [ESC] when displaying monster list, return back to player control
    - Q quit game
   **DISPLAY IS LINES 1-21, USE 0 TO DISPLAY ERROR MESSAGES
**/


//feel free to change the name for this, not really sure if this is needed, its just me trying to figure out ncurses
void pc_commands(dungeon_t *d, int key) {
    //getch(); pauses to wait for an input, have to press enter i think?
    //printw(...); prints stuff to the screen
    //refresh(); updates the screen

    pair_t dir;
    switch(key) {

        //move pc to upper left?
        case '7':
        case 'y':
        case KEY_HOME:
            dir[dim_y] = -1;
            dir[dim_x] = -1;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell up
        case '8':
        case 'k':
        case KEY_UP:
            dir[dim_y] = -1;
            dir[dim_x] = 0;
            pc_next_pos(&d, dir);
            break;

        //move pc to upper right
        case '9':
        case 'u':
        case KEY_PPAGE:
            dir[dim_y] = -1;
            dir[dim_x] = 1;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell to right
        case '6':
        case '1':
        case KEY_RIGHT:
            dir[dim_y] = 0;
            dir[dim_x] = 1;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell to lower right
        case '3':
        case 'n':
        case KEY_NPAGE:
            dir[dim_y] = 1;
            dir[dim_x] = 1;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell DOWN
        case '2':
        case 'j':
        case KEY_DOWN:
            dir[dim_y] = 1;
            dir[dim_x] = 0;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell to lower left
        case '1':
        case 'b':
        case KEY_END:
            dir[dim_y] = 1;
            dir[dim_x] = -1;
            pc_next_pos(&d, dir);
            break;

        //move pc one cell to the left
        case '4':
        case 'h':
        case KEY_LEFT:
            dir[dim_y] = 0;
            dir[dim_x] = -1;
            pc_next_pos(&d, dir);
            break;

        //attempt to go down stairs. Works only if standing on down staircase.
        case '>':
        //if pc is going downstairs
            //delete pc
            pc_delete(d->pc.pc); // I think thats how you get pc from current dungeon
            //delete current dungeon
            delete_dungeon(&d);
            //generate new dungeon
            init_dungeon(&d);
            gen_dungeon(&d);
            //generate new pc
            config_pc(&d);
            //generate gen_monsters
            gen_monsters(&d);
            //renders dungeon
            render_dungeon(d);
            break;

        //attempt to go up stairs. Works only if standing on up staircase.
        case '<':
        //if pc is going upstairs
            //delete pc
            pc_delete(d->pc.pc); // I think thats how you get pc from current dungeon
            //delete current dungeon
            delete_dungeon(&d);
            //generate new dungeon
            init_dungeon(&d);
            gen_dungeon(&d);
            //generate new pc
            config_pc(&d);
            //generate gen_monsters
            gen_monsters(&d);
            //renders dungeon
            render_dungeon(d);
            break;

        //npcs still move, x and y = 0
        case '5':
        case ' ':
        case KEY_B2:
            dir[dim_y] = 0;
            dir[dim_x] = 0;
            pc_next_pos(&d, dir);
            break;
        case 'Q':
            //delete pc
            pc_delete(d.pc.pc);

            //delete dungeon
            delete_dungeon(d);
            endwin(); //close stdscr
            abort();
            break; //we put break here right? I'm not sure we need a break cause of abort

        default:
            return 0;
    }
}


uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  // while(!pc_commands(&d, dir)) {
  //   return 0;
  // }

  d->pc.position[dim_x] = dir[dim_x];
  d->pc.position[dim_y] = dir[dim_y];

  // if (target_is_valid &&
  //     (d->pc.position[dim_x] == d->rooms[target_room].position[dim_x]) &&
  //     (d->pc.position[dim_y] == d->rooms[target_room].position[dim_y])) {
  //   target_is_valid = 0;
  // }

  //dir[dim_y] = dir[dim_x] = 0;

  return 0;
}
