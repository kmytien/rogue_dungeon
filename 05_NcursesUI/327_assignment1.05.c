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
	CS 327: Assignment 1.05 - NCurses UI
	By: MyTien Kien, Sanjana Amatya, Haylee Lawrence
**/


//stairs function that regenerates a different dungeon when going up or down stairs
int stairs(dungeon_t *d) {

    //delete pc
    pc_delete(d->pc.pc); 
    
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
}


//all key inputs
uint32_t key_inputs(dungeon_t *d, pair_t dir) {
    static uint32_t count = 0;

    mvprintw(0, 20, "%s", "Press a key to move the PC in a direction.");

    int input; 
    bool cont = false;
    
    while (!cont) {
        switch (input = getch()) {
        
            //move pc to upper left
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

            case '>':
            //if pc is going downstairs
                if (d->map[d->pc.position[dim_y]][d->pc.position[dim_x]] == ter_stairs_down) {
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

            //npcs still move, pc stays still
            case '5':
            case ' ':
            case '.':
            case KEY_B2:
                dir[dim_y] = 0;
                dir[dim_x] = 0;
                //pc_next_pos(d, dir);
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
                endwin(); 
                abort();
                break; 
            
            default:
                mvprintw(0, 20, "%-40s", "	ERROR: Please press a valid key.	");
        }
    }

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
	
    display_monster_list(d, monsters, slot);
    display_nc_dungeon(d);
    free(monsters);
}


//io to display the list on terminal
//display a list of monsters in the dungeon, with their symbol and position relative to the PC (e.g.: “c, 2 north and 14 west”).
void display_monster_list (dungeon_t *d, character_t **monsters, uint32_t slot){
    uint32_t count = slot;
	char *spaces = "                                           ";
    
    mvprintw(3, 18, "%s", spaces);
    mvprintw(4, 18, "  You know of %d monsters:                  ", count);
    mvprintw(5, 18, "%s", spaces);
    
    print_list(d, monsters, 0, count);
	
    if (count > 10) {
        mvprintw(16, 18, "%s", spaces);
        mvprintw(17, 18, "%s", "  Arrows to scroll & ESC to go back        ");
        mvprintw(18, 18, "%s", spaces);
        scroll_monster_list(d, monsters, count);	    
    }
    
    else {
        mvprintw(count + 6, 18, "%s", spaces);
        mvprintw(count + 7, 18, "%s", "  Press the escape button to continue        ");
        mvprintw(count + 8, 18, "%s", spaces);
        print_list(d, monsters, 0, count);
        
        //as long as user doesn't hit escape continue to else
        while(getch() != 27);
    }
}


//scrolling through monster list
void scroll_monster_list(dungeon_t *d, character_t **monsters, uint32_t count){

    uint32_t i, num = 10;
    bool foo = true;

    do {
        switch(getch()) {
        
            case KEY_UP:
                if (num > 10) {
                	num--;
                	print_list(d, monsters, num - 10, num);
                }
                
                break;

            case KEY_DOWN:
                if (num < count) { 
                	num++;
                	print_list(d, monsters, num - 10, num);
                }
                
                break;

            case 27:
              foo = false;
              break;
        }
        
    } while (foo);
    
    return;
}


//prints the monster list
void print_list(dungeon_t *d, character_t **monsters, uint32_t begin, uint32_t end) {
    char *north_south;
    char *west_east;
    
    uint32_t i, line = 0;
    int mx, my;
    int NS_dir, WE_dir;
    int pc_x = d->pc.position[dim_x], pc_y = d->pc.position[dim_y];
	
	for (i = begin, line = 0; i < end && line < 10; i++, line++) {
		
	    //print how far away from pc the monster is
	    mx = monsters[i]->position[dim_x];
	    my = monsters[i]->position[dim_y];

	    if (pc_x - mx >= 0) west_east = "WEST";
	    else west_east = "EAST";
	    WE_dir = abs(pc_x - mx);

	    if (pc_y - my >= 0) north_south = "NORTH";
	    else north_south = "SOUTH";
	    NS_dir = abs(pc_y - my);
		
	    mvprintw(line + 6, 18, "  The monster %c is %3d %s and %3d %s  ",
		     monsters[i]->symbol, NS_dir, north_south, WE_dir, west_east);
	}
}


//new render dungeon that prints to the ncurse screen
//used sheaffer's render_dungeon and just changed putchar to mvaddch
void display_nc_dungeon(dungeon_t *d) {
    pair_t p;
    clear();

    putchar('\n');
    for (p[dim_y] = 0; p[dim_y] < DUNGEON_Y; p[dim_y]++) {
        for (p[dim_x] = 0; p[dim_x] < DUNGEON_X; p[dim_x]++) {
            if (charpair(p)) {
                mvaddch(p[dim_y] + 1, p[dim_x], charpair(p)->symbol);
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
    
    mvaddch(p[dim_y] + 1, p[dim_x], '\n');
    mvaddch(p[dim_y] + 1, p[dim_x], '\n');
    refresh();
}
