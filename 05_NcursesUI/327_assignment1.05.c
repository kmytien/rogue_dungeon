#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>

#include "move.h"
#include "path.h"
#include "pc.h"
#include "utils.h"
#include "dungeon.h"
#include "assignment_105.h"
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


int stairs(dungeon_t *d) {
    //delete pc
    pc_delete(d->pc.pc); // I think thats how you get pc from current dungeon
    //delete current dungeon
    delete_dungeon(d);
    //generate new dungeon
    init_dungeon(d);
    gen_dungeon(d);
    //generate new pc
    config_pc(d);
    d->character[d->pc.position[dim_y]][d->pc.position[dim_x]] = &d->pc;
    //generate gen_monsters
    gen_monsters(d);
    
    //renders dungeon
    //display_render_dungeon(&d);
}


//pc next position
uint32_t pc_next_pos(dungeon_t *d, pair_t dir) {
    static uint32_t have_seen_corner = 0;
    static uint32_t count = 0;

    mvprintw(0, 0, "%-40s", "Press a key to move the PC in a direction.");

    int input = getch();
    bool cont = false;

    while (!cont) {
        switch (input) {
            //move pc to upper left?
            case '7':
            case 'y':
            case KEY_HOME:
                dir[dim_y] = -1;
                dir[dim_x] = -1;
                cont = true;
                break;

            //move pc one cell up
            case '8':
            case 'k':
            case KEY_UP:
                dir[dim_y] = -1;
                dir[dim_x] = 0;
                cont = true;
                break;

            //move pc to upper right
            case '9':
            case 'u':
            case KEY_PPAGE:
                dir[dim_y] = -1;
                dir[dim_x] = 1;
                cont = true;
                break;

            //move pc one cell to right
            case '6':
            case 'l':
            case KEY_RIGHT:
                dir[dim_y] = 0;
                dir[dim_x] = 1;
                cont = true;
                break;

            //move pc one cell to lower right
            case '3':
            case 'n':
            case KEY_NPAGE:
                dir[dim_y] = 1;
                dir[dim_x] = 1;
                cont = true;
                break;

            //move pc one cell DOWN
            case '2':
            case 'j':
            case KEY_DOWN:
                dir[dim_y] = 1;
                dir[dim_x] = 0;
                cont = true;
                break;

            //move pc one cell to lower left
            case '1':
            case 'b':
            case KEY_END:
                dir[dim_y] = 1;
                dir[dim_x] = -1;
                cont = true;
                break;

            //move pc one cell to the left
            case '4':
            case 'h':
            case KEY_LEFT:
                dir[dim_y] = 0;
                dir[dim_x] = -1;
                cont = true;
                break;

            //attempt to go down stairs. Works only if standing on down staircase.
            case '>':
                if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_up) {
                    stairs(d);
                    cont = true;
                    d->is_stairs = 1;
                }
                break;
            case '<':
            //if pc is going upstairs
                if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_up) {
                    stairs(d);
                    cont = true;
                    d->is_stairs = 1;
                }
                break;

            //npcs still move, x and y = 0
            case '5':
            case ' ':
            case '.':
            case KEY_B2:
                dir[dim_y] = 0;
                dir[dim_x] = 0;
                pc_next_pos(d, dir);
                cont = true;
                break;
            case 'm':
                create_monster_list(d);
                break;

            case 'Q':
                //delete pc
                pc_delete(d->pc.pc);

                //delete dungeon
                delete_dungeon(d);
                endwin(); //close stdscr
                abort();
                break; //we put break here right? I'm not sure we need a break cause of abort
            
            default:
                mvprintw(0, 0, "%-40s", "ERROR: Please press a valid key.");
                break;
        }
    }

//   pair_t next_pos;
//   d->pc.position[dim_x] += dir[dim_x];
//   d->pc.position[dim_y] += dir[dim_y];
//   next_pos[dim_y] = d->pc.position[dim_y];
//   next_pos[dim_x] = d->pc.position[dim_x];
//   move_character(d, &d->pc, next_pos);

  return 0;
}


//creating a list of all the monsters
void create_monster_list(dungeon_t *d) {

    character_t **monsters;
    uint32_t slot = 0;
    monsters = malloc(d->num_monsters * sizeof(*monsters));

    for (int i = 0; i < DUNGEON_Y; i++) {
        for (int j = 0; j < DUNGEON_X; j++) {
            if (d->character[i][j] != &d->pc && d->character[i][j] != NULL) {
                monsters[slot] = d->character[i][j];
                slot++;
            }
        }
    }

    display_monster_list(d, monsters);
    display_render_dungeon(d);
    free(monsters);
}


//code to scroll down the monster list
void scroll_monster_list(dungeon_t *d, uint32_t count){

    uint32_t num = 0;
    bool foo = true;

    while (foo) {
    	//for (i = 0; i < 13; i++) {
        //    mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
        //}
    
        switch(getch()) {
            case KEY_UP:
                if (num) num--;
                break;

            case KEY_DOWN:
                if (num < (count - 10)) num++;
                break;

            case 27:
              foo = false;
              return;
        }
    }
}


//io to display the list on terminal
//Display a list of monsters in the dungeon, with their symbol and position relative to the PC (e.g.: “c, 2 north and 14 west”).
void display_monster_list(dungeon_t *d, character_t **monsters){
    char(*x)[40];
    uint32_t count = (d->num_monsters * sizeof(*monsters));

    char *north_south;
    char *west_east;

    x = malloc(count * sizeof(*x));

    mvprintw(1, 19, "%-40s", " ");
    snprintf(x[0], 40, "You know of %d monsters:", count);

    mvprintw(2, 19, "%-40s", x);
    mvprintw(3, 19, "%-40s", " ");

    uint32_t i;
    int pc_x = d->pc.position[dim_x], pc_y = d->pc.position[dim_y];
    int mx, my;
    int NS_dir, WE_dir;

    for (i = 0; i < count; i++) {
        //print how far away from pc the monster is
        mx = monsters[i]->position[dim_x];
        my = monsters[i]->position[dim_y];

        if (pc_x - mx >= 0) west_east = "WEST";
        else west_east = "EAST";
        WE_dir = pc_x - mx;

        if (pc_y - my >= 0) north_south = "NORTH";
        else north_south = "SOUTH";
        NS_dir = pc_y - my;

        mvprintw(i + 4, 19, "The monster %c is %d %s and %d %s",
                 monsters[i]->symbol, NS_dir, north_south, WE_dir, west_east);

        if (count < 11) mvprintw(i + 4, 19, "%-40s", x[i]);
    }

    if (count < 11) {
        mvprintw(count + 4, 19, "%-40s", " ");
        mvprintw(count + 5, 19, "%-40s", "Press the escape button to continue.");
        //as long as user doesn't hit escape continue to else
    }

    else {
        mvprintw(18, 19, "%-40s", " ");
        mvprintw(19, 19, "%-40s", "Arrows allow you to scroll, press the escape button to continue.");
        scroll_monster_list(d, count);
    }

    free(x);
}

// new render dungeon that prints to the ncurse screen
void display_render_dungeon(dungeon_t *d) {
    pair_t p;
    clear();

    putchar('\n');
    for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
        for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
            if (charpair(p)) {
                mvaddch(p[dim_y] + 1, p[dim_x], charpair(p)->symbol); //this is where it stops printing..
            }

            else {
                switch (mappair(p)) {

                    case ter_wall:
                    case ter_wall_immutable:
                        mvaddch(p[dim_y] + 1, p[dim_x], ' ');
                        break;

                    case ter_floor:
                    case ter_floor_room:
                        mvaddch(p[dim_y] + 1, p[dim_x], '.');
                        break;

                    case ter_floor_hall:
                        mvaddch(p[dim_y] + 1, p[dim_x], '#');
                        break;

                    case ter_debug:
                        mvaddch(p[dim_y] + 1, p[dim_x], '*');
                        fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
                        break;

                    case ter_stairs_up:
                        mvaddch(p[dim_y] + 1, p[dim_x], '<');
                        break;

                    case ter_stairs_down:
                        mvaddch(p[dim_y] + 1, p[dim_x], '>');
                        break;

                    default:
                        mvaddch(p[dim_y] + 1, p[dim_x], ';');
                }
            }
        }
    }
    
    //printf("\n\nhey\n\n");
    mvaddch(p[dim_y] + 1, p[dim_x], '\n');
    mvaddch(p[dim_y] + 1, p[dim_x], '\n');
    refresh();
}
