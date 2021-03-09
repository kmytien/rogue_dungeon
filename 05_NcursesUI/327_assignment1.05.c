#include <stdio.h>
#include <ncurses.h>

#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"
#include "npc.h"
#include "character.h"


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
int pc_commands(dungeon_t *d, int key) {
    //getch(); pauses to wait for an input, have to press enter i think?
    //printw(...); prints stuff to the screen
    //refresh(); updates the screen

    pair_t dir;
    char input = getch();
    uint32_t escape_key;

    do{
      if((key == getch()) == 27) {
          display_render_dungeon(d);
          return;
      }
    }

    switch(input) {

        //move pc to upper left?
        case '7':
        case 'y':
        case KEY_HOME:
            dir[dim_y] = -1;
            dir[dim_x] = -1;
            pc_next_pos(d, dir);
            break;

        //move pc one cell up
        case '8':
        case 'k':
        case KEY_UP:
            dir[dim_y] = -1;
            dir[dim_x] = 0;
            pc_next_pos(d, dir);
            break;

        //move pc to upper right
        case '9':
        case 'u':
        case KEY_PPAGE:
            dir[dim_y] = -1;
            dir[dim_x] = 1;
            pc_next_pos(d, dir);
            break;

        //move pc one cell to right
        case '6':
        case 'l':
        case KEY_RIGHT:
            dir[dim_y] = 0;
            dir[dim_x] = 1;
            pc_next_pos(d, dir);
            break;

        //move pc one cell to lower right
        case '3':
        case 'n':
        case KEY_NPAGE:
            dir[dim_y] = 1;
            dir[dim_x] = 1;
            pc_next_pos(d, dir);
            break;

        //move pc one cell DOWN
        case '2':
        case 'j':
        case KEY_DOWN:
            dir[dim_y] = 1;
            dir[dim_x] = 0;
            pc_next_pos(d, dir);
            break;

        //move pc one cell to lower left
        case '1':
        case 'b':
        case KEY_END:
            dir[dim_y] = 1;
            dir[dim_x] = -1;
            pc_next_pos(d, dir);
            break;

        //move pc one cell to the left
        case '4':
        case 'h':
        case KEY_LEFT:
            dir[dim_y] = 0;
            dir[dim_x] = -1;
            pc_next_pos(d, dir);
            break;

        //attempt to go down stairs. Works only if standing on down staircase.
        case '>':
        //if pc is going downstairs
            stairs(d);
            break;

        //attempt to go up stairs. Works only if standing on up staircase.
        case '<':
        //if pc is going upstairs
            stairs(d);
            break;

        //npcs still move, x and y = 0
        case '5':
        case ' ':
        case KEY_B2:
            dir[dim_y] = 0;
            dir[dim_x] = 0;
            pc_next_pos(&d, dir);
            break;

        case 'm':
          create_monster_list(d);

        case 'Q':
            //delete pc
            pc_delete(d->pc.pc);

            //delete dungeon
            delete_dungeon(d);
            endwin(); //close stdscr
            abort();
            break; //we put break here right? I'm not sure we need a break cause of abort

        default:
            return 0;
    }
}


int stairs(dungeon_t *d) {
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
}


//pc next position
uint32_t pc_next_pos(dungeon_t *d, pair_t dir) {
  static uint32_t have_seen_corner = 0;
  static uint32_t count = 0;

  while(!pc_commands(&d, dir)) { // this is gonna be an issue --probably :'(
    return 0;
  }

  d->pc.position[dim_x] = dir[dim_x];
  d->pc.position[dim_y] = dir[dim_y];

  return 0;
}


//creating a list of all the monsters
void create_monster_list(dungeon_t *d) {

    character_t *monsters;
    uint32_t slot = 0;
    monsters = malloc(d->num_monsters * sizeof(*monsters));

    for (int i = 0; i < DUNGEON_Y; i++) {
        for (int j = 0; i < DUNGEON_X; j++) {
            if (d->character[i][j] != d->pc && d->character[i][j] != NULL) {
                monsters[slot] = d->character[i][j];
                slot++;
            }
        }
    }

    display_monster_list(d, monster, slot);
    free(monsters);
}


//code to scroll down the monster list
void scroll_monster_list(dungeon_t *d, uint32_t count){

    uint32_t num = 0;

    switch(getch()) {
        case KEY_UP:
            if (num) num--;
            break;

        case KEY_DOWN:
            if (num < (count - 10)) num++;
            break;

        case 27:
          return;
    }
}


//io to display the list on terminal
//Display a list of monsters in the dungeon, with their symbol and position relative to the PC (e.g.: “c, 2 north and 14 west”).
void display_monster_list(dungeon_t *d, character_t *monsters){
    char(*x)[40];
    uint32_t count = (d->num_monsters * sizeof(*monsters));

    char north_south[6];
    char west_east[5];

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

        //print ------------------------------------------------- idk how to do this
        //mvprintw(x, y(index of where you want it to be on terminal), 40 char from x array , message or value you want to print);
        mvprintw(i + 4, 19, "%-40s", x[i]);
    }

    if (count <= 10) {
        mvprintw(count + 4, 19, "%-40s", " ");
        mvprintw(count + 5, 19, "%-40s", "Press the escape button to continue.");
        //as long as user doesn't hit escape continue to else
    } else {
        mvprintw(18, 19, "%-40s", " ");
        mvprintw(19, 19, "%-40s", "Arrows allow you to scroll, press the escape button to continue.");
        scroll_monster_list(d, count);
    }
    free(x);
}

// new render dungeon that prints to the ncurse screen
void display_render_dungeon(dungeon_t *d)
{
  pair_t p;

  putchar('\n');
  for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
    for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
      if (charpair(p)) {
        mvaddch(charpair(p)->symbol);
      } else {
        switch (mappair(p)) {
        case ter_wall:
        case ter_wall_immutable:
          mvaddch(p[dim_y] + 1, p[dim_x], ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(p[dim_y] + 1, p[dim_x],'.');
          break;
        case ter_floor_hall:
          mvaddch(p[dim_y] + 1, p[dim_x],'#');
          break;
        case ter_debug:
          mvaddch(p[dim_y] + 1, p[dim_x],'*');
          fprintf(stderr, "Debug character at %d, %d\n", p[dim_y], p[dim_x]);
          break;
        case ter_stairs_up:
          mvaddch(p[dim_y] + 1, p[dim_x],'<');
          break;
        case ter_stairs_down:
          mvaddch(p[dim_y] + 1, p[dim_x],'>');
          break;
        default:
          mvaddch(p[dim_y] + 1, p[dim_x], '0');
        }
      }
    }
    mvaddch(p[dim_y] + 1, p[dim_x],'\n');
  }
  mvaddch('\n');
  mvaddch('\n');
  refresh();
}
