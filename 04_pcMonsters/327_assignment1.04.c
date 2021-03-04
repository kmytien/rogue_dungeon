//include statements below
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "dungeon.h"
#include "dims.h"
#include "heap.h"
#include "path.h"
#include "utils.h"

/**
    NOTES:
      *** ill make seperate header file for this after we finish, the main function will be in rlg - MK ***
      *** may interpret these however we like ***
      - Need to move monsters around
          - "--nummon [INTEGER]" summons an INTEGER number of monsters around (10 is reasonable) DONE
          - Also have a hard coded default in case no --nummon
          - Represented with letters and occasionally letters/punctuation
          - Monsters may have 50% chance of having the following:
                - Intelligence: understands dungeon layout and moves on shortest path, can remember last known position
                  of previously seen PC
                - Unintelligent: monsters move in a straight line toward PC
                - Telepathy: always know where PC is, moves toward PC
                - Non-Telepathic: knows where PC is if PC is in line of sight, moves toward last known position
                - Tunneling Ability: can tunnel through rock
                - Erratic Behavior: have a 50% chance of moving as per their other characteristics,
                  otherwise they move to a random neighboring cell
      - Speed
        - Each monsters gets speed between 5 and 20
        - PC gets speed of 10
      - Basically whats supposed to happen in main(?) is
        - all monsters move (based on type) towards a (stationary) pc
        - when 2 characters land on the same space, the newcomer kills the original player on the cell
        - the game continues until the player dies or all the monsters are dead
        - (use usleep(3) which sleeps for an argument number of microseconds (250000ish) and when the game ends, prints win/lose status before exiting)
**/

char lose_message[12] = "you lost lol";
char win_message[28] = "you won. which is surprising";

struct tunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    int32_t cost;
} tunnel_t;

struct nontunnel {
    heap_node_t *hn;
    uint8_t pos[2];
    int32_t cost;
} nontunnel_t;


// struct player {
//
// } pc;
//
// struct nonplayer {
//
// } npc;



typedef struct m_path {
  heap_node_t *hn;
  uint8_t pos[2];
} mp_t;

// struct character {
//     pair_t position;
//     pair_t next_pos;
//
//     union {
//         struct player;
//         struct nonplayer;
//         bool is_alive;
//     };
// };

// how do we figure out the number of monsters we need? (argc)
void generate_monsters(dungeon_t *d, char m) {
    int i, j, type[4], x, y;
    bool valid = false;

    // init mons arrays
    for (y = 1; y < 21; y++) {
      for (x = 1; x < 80; x++) {
        d->mons[y][x] = ' '; // int array, might need to be changed
      }
    }

    // generate needed amnt of monsters
    for (i = 0; i < d->num_monsters; i++) {
        // find monster position that is on floor and is not the pc and place it there
        while (!valid) {
            x = (rand() % 80) + 1;
            y = (rand() % 20) + 1;

            if (d->map[y][x] == ter_floor && d->pc.position[dim_x] != x && d->pc.position[dim_y] != y) {
                valid = true;
                d->monsters[i].position[dim_x] = x;
                d->monsters[i].position[dim_y] = y;
            }
        }

        // generate hexidecimal monster type and set as char m
        // do we need srand(time(NULL))?
        int mon = rand() % 15;
        if (mon <= 9)
            d->monsters[i].m = mon;

        else {
            if (mon == 10) d->monsters[i].m = 'a';
            else if (mon == 11) d->monsters[i].m = 'b';
            else if (mon == 12) d->monsters[i].m = 'c';
            else if (mon == 13) d->monsters[i].m = 'd';
            else if (mon == 14) d->monsters[i].m = 'e';
            else d->monsters[i].m = 'f';
        }

        d->mons[d->monsters[i].position[dim_y]][d->monsters[i].position[dim_x]] = i+1;

        // convert to binary
        for (j = 0; mon > 0; j++) { // type is stored in an array of 4 numbers
            type[j] = mon % 2;
            mon /= 2;
        }

        // set struct levels
        d->monsters[i].mon_type[0] = type[0]; // sets erratic behavior
        d->monsters[i].mon_type[1] = type[1]; // sets tunneling
        d->monsters[i].mon_type[2] = type[2]; // sets telepathy
        d->monsters[i].mon_type[3] = type[3]; // sets intelligence
        d->monsters[i].speed = (rand() % 20) + 5; //set speed

        //figure out the last known position of the monster?
        if(d->monsters[i].mon_type[3] == 1){ //how does one make sure intelligence is equal to one
          d->monsters[i].last_pos[0] = 0; //sorry idk how to do the inteligece stuff
          d->monsters[i].last_pos[1] = 0;
        }

        // set sequence
        d->monsters[i].sequence = i + 1;

        // set next_turns
        d->monsters[i].next_turn = 0;
    }
}

//boolean decides if the game has been won or lost and prints if the game is done
bool game_done(dungeon_t *d) {
    int i;
    //game ends if pc is dead
    if (d->pc.is_alive) {
        printf(lose_message);
        return false;
    }

    //or if all monsters are dead
    for (i = 0; i < d->num_monsters; i++) {
        if (d->monsters[i].is_alive) {
            printf(win_message);
            return false;
        }
    }

    return true;
}

//for unintelligent monsters (to move straight)
void straight(monster_t *monster, dungeon_t *d) {

    if (monster->position[dim_x] < d->pc.position[dim_x]) {
        monster->next_pos[dim_y] = monster->position[dim_y];
        monster->next_pos[dim_x] = monster->position[dim_x] + 1;
    }

    else if (monster->position[dim_x] > d->pc.position[dim_x]) {
        monster->next_pos[dim_y] = monster->position[dim_y];
        monster->next_pos[dim_x] = monster->position[dim_x] - 1;
    }

    else if (monster->position[dim_y] < d->pc.position[dim_y]) {
        monster->next_pos[dim_y] = monster->position[dim_y] + 1;
        monster->next_pos[dim_x] = monster->position[dim_x];
    }

    else if (monster->position[dim_y] > d->pc.position[dim_y]) {
        monster->next_pos[dim_y] = monster->position[dim_y] - 1;
        monster->next_pos[dim_x] = monster->position[dim_x];
    }
}


//for non telepathic & non tunneling monsters if they see pc in line of sight
//so most likely if they're in a room
//might use Bresenham's line drawing algorithm
int in_line_of_sight(dungeon_t *d, monster_t *monster) {

    int monster_y = monster->position[dim_y], monster_x = monster->position[dim_x];
    //if pc is left/above monster or right/below monster
    bool y_negative = false, x_negative = false;
    int x_dir, y_dir, i1, i2, i3;

    if (monster_x > d->pc.position[dim_x]) {
        x_negative = true;
        x_dir = monster_x - d->pc.position[dim_x];
    } else x_dir = d->pc.position[dim_x] - monster_x;

    if (monster_y > d->pc.position[dim_y]) {
        y_negative = true;
        y_dir = monster_y = d->pc.position[dim_y];
    } else y_dir = d->pc.position[dim_y] - monster_y;

    //finishing later -mk
    if (y_dir > x_dir) {
        i1 = x_dir * 2;
        i2 = i1 - y_dir;
        i3 = i2 - y_dir;

        for (int i = 0; i < y_dir + 1; i++) {
            //checks through y direction if all spots isn't a floor tile (rock -> NOT in line of sight)
            if ((d->map[monster_y][monster_x] < ter_floor) && (i != y_dir))
                return 0;

            //calculating coords for next pixel
            //i do not get the math in this but this was from a tutorial for line algorithm lol -mk
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                monster_x++;
            }
        }
        return 1;
    } else {
        i1 = y_dir * 2;
        i2 = i1 - x_dir;
        i3 = i2 - x_dir;

        for (int i = 0; i < x_dir + 1; i++) {
            //checks through y direction if all spots isn't a floor tile (rock -> NOT in line of sight)
            if ((d->map[monster_y][monster_x] != ter_floor) && (i != x_dir))
                return 0;

            //calculating coords for next pixel
            //i do not get the math in this but this was from a tutorial for line algorithm lol -mk
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                monster_y++;
            }
        }
        return 1;
    }
}


//implementing dijkstra for tuneeling and non tunneling monsters
void shortest_path(monster_t* monster, dungeon_t* d) {

    //monster x, y positions
    int x = 0, y = 0, num = INT_MAX;
    int monster_x = monster->position[dim_x], monster_y = monster->position[dim_y];
    bool tunneling = false;
    //if statement for tunneling -----------------------------------------------------------need to do

    //need to use pc_tunnel[][] that sheaffer used
    if (tunneling) {
        //need to initialize variables
        //do stuff idk
        //cost > (d->pc_tunnel[monster_y][monster_x] + d->hardness[monster_y][monster_x] / 85)

        //cardinal directions
        if (num > d->pc_tunnel[monster_y - 1][monster_x]) {
            y = -1;
            num = d->pc_tunnel[monster_y - 1][monster_x];
        }

        if (num > d->pc_tunnel[monster_y + 1][monster_x]) {
            y = 1;
            num = d->pc_tunnel[monster_y + 1][monster_x];
        }

        if (num > d->pc_tunnel[monster_y][monster_x - 1] < num) {
            x = -1;
            num = d->pc_tunnel[monster_y][monster_x - 1];
        }

        if (num > d->pc_tunnel[monster_y][monster_x + 1]) {
            x = 1;
            num = d->pc_tunnel[monster_y][monster_x + 1];
        }
    }


    else {
        if (d->pc_distance[monster->position[dim_y] - 1][monster->position[dim_x]] < num) {
            x = -1;
            num = d->pc_distance[monster->position[dim_y] - 1][monster->position[dim_x]];
        }

        if (d->pc_distance[monster->position[dim_y] + 1][monster->position[dim_x]] < num) {
            x = 1;
            num = d->pc_distance[monster->position[dim_y] + 1][monster->position[dim_x]];
        }

        if (d->pc_distance[monster->position[dim_y]][monster->position[dim_x] + 1] < num) {
            y = 1;
            num = d->pc_distance[monster->position[dim_y]][monster->position[dim_x] + 1];
        }

        if (d->pc_distance[monster->position[dim_y]][monster->position[dim_x] - 1] < num) {
            y = -1;
            num = d->pc_distance[monster->position[dim_y]][monster->position[dim_x] - 1];
        }
    }

    //how do we actually update monster movements
    //add x and y? i think we use their pos
    //cause i put pos in their structs so we can do pos[1] - 1 noice yeeee :)
    monster_x += x;
    monster_y += y;
}


// moves the monsters for all possibilities
void move(monster_t *monster, dungeon_t *d, heap_t *heap) {
    //I think we have to use nextpos
    //pc
    // remove monster from monster map
    d->mons[monster->position[dim_y]][monster->position[dim_x]] = ' ';

    if (monster->m == '@') {
      // moves towards pc if monster sees them??
      //I think pc moves randomly and monster position depends on the pc position
      monster->next_pos[dim_y] = monster->position[dim_y] + d->pc.position[dim_y];
      monster->next_pos[dim_x] = monster->position[dim_x]+ d->pc.position[dim_x];

      //not erratic, not tunneling, not telepathic, not smart
    } else if (monster->m == '0') {
        //if pc is in line of sight -> move towards pc
        if (in_line_of_sight)
            straight(monster, d);

        else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }

      //not erratic, not tunneling, not telepathic, smart
    } else if (monster->m == '1') {
          if (in_line_of_sight)
              straight(monster, d);

          else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }

      //not erratic, not tunneling, telepathic, not smart
    } else if (monster->m == '2') {
          if (in_line_of_sight)
              straight(monster, d);

          else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }

      //not erratic, not tunneling, telepathic, smart
    } else if (monster->m == '3') {

      // telepathic monsters constantly know where the pc is
      // always move toward the pc
      shortest_path(monster, d);
      /**
          if (in_line_of_sight)
              straight(monster, d);

          else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }
      **/

      //not erratic, tunneling, not telepathic, not smart
    } else if (monster->m == '4') {
          if (in_line_of_sight)
              straight(monster, d);

          //i will figure this out
          else {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

              if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                  hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                  d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                  dijkstra_tunnel(d);
                  dijkstra(d);
              }
          }

      //not erratic, tunneling, not telepathic, smart
    } else if (monster->m == '5') {
          if (in_line_of_sight)
              straight(monster, d);

          //i will figure this out
          else {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

              if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                  hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                  d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                  dijkstra_tunnel(d);
                  dijkstra(d);
              }
          }

      //not erratic, tunneling, telepathic, not smart
    } else if (monster->m == '6') {
      /*
      if (in_line_of_sight)
          straight(monster, d);
      */

          int rand_x = (rand() % 2) - 1; //range from -1 to 1?
          int rand_y = (rand() % 2) - 1;

          monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
          monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
              hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
              d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

              dijkstra_tunnel(d);
              dijkstra(d);
          }

      //not erratic, tunneling, telepathic, smart
    } else if (monster->m == '7') {
          shortest_path(monster, d); //idk -- does shortestpath let the monster know where the pc is?

          /*
          int rand_x = (rand() % 2) - 1; //range from -1 to 1?
          int rand_y = (rand() % 2) - 1;

          monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
          monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
              hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
              d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

              dijkstra_tunnel(d);
              dijkstra(d);
          }
          */

      //erratic, not tunneling, not telepathic, not smart
    } else if (monster->m == '8') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight) {
              straight(monster, d);

          } else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }

      //erratic, not tunneling, not telepathic, smart
    } else if (monster->m == '9') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight) {
              //shortest_path(monster, d);
              straight(monster, d);

          } else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }
      //erratic, not tunneling, telepathic, not smart
    } else if (monster->m == 'a') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
            int rand_x = (rand() % 2) - 1; //range from -1 to 1?
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight){
              //do tunneling stuff
              //shortest_path(monster, d);
              straight(monster, d);

          } else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }
      //erratic, not tunneling, telepathic, smart
    } else if (monster->m == 'b') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
            int rand_x = (rand() % 2) - 1; //range from -1 to 1?
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else {
              //do tunneling stuff
              shortest_path(monster, d);
              //straight(monster, d);

          } /*else {
              monster->next_pos[dim_y] = monster->position[dim_y];
              monster->next_pos[dim_x] = monster->position[dim_x];
          }*/


      //erratic, tunneling, not telepathic, not smart
    } else if (monster->m == 'c') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
            int rand_x = (rand() % 2) - 1; //range from -1 to 1?
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight) {
              straight(monster, d);

          } else {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

              if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                  hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                  d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                  dijkstra_tunnel(d);
                  dijkstra(d);
              }
          }

      //erratic, tunneling, not telepathic, smart
    } else if (monster->m == 'd') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
            int rand_x = (rand() % 2) - 1; //range from -1 to 1?
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight) {
              shortest_path(monster, d);

          } else {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

              if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                  hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                  d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                  dijkstra_tunnel(d);
                  dijkstra(d);
              }
          }

      //erratic, tunneling, telepathic, not smart
    } else if (monster->m == 'e') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          } else if (in_line_of_sight){
            //do tunneling stuff
                straight(monster, d);

          } else {
                int rand_x = (rand() % 2) - 1; //range from -1 to 1?
                int rand_y = (rand() % 2) - 1;

                monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
                monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

                if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                    hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                    d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                    dijkstra_tunnel(d);
                    dijkstra(d);
                }
          }

      //erratic, tunneling, telepathic, smart (f) --- i do not know what to do for this one and 7 -- which part of it are you talking about?
      //like idk what to do in general for telepathic, smart and tunneling all together so for smart we just use the shortest_path cause the function does everything
      //for the monster to get to the pc the fastest and it looks like you got tunneling but I'm not sure what the ran_x and rand_y are for? 
      //for erratic, he said it would either do the other characteristics or move to a random space, so i added a rand_x and rand_y
      //for the monster to move to :P  haylee wants to update so i let her update - do we have to update in tunneling too? what in tunnelling wym 
      //-- cause you have it in the else statement too
    } else if (monster->m == 'f') {
          int is_erratic = rand() % 2;

          if (is_erratic) {
            int rand_x = (rand() % 2) - 1; //range from -1 to 1?
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

          }else if (in_line_of_sight){
              shortest_path(monster, d);
          }else {
              int rand_x = (rand() % 2) - 1; //range from -1 to 1?
              int rand_y = (rand() % 2) - 1;

              monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
              monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

              if (hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) <= 85) {
                  hardnessxy(monster->next_pos[dim_x], monster->next_pos[dim_y]) = 0;
                  d->map[monster->next_pos[dim_y]][monster->next_pos[dim_x]] = ter_floor_hall;

                  dijkstra_tunnel(d);
                  dijkstra(d);
              }
          }
    }

    //knowing which monster's turn it is
    //something with heap_insert
    //error bc its been called in run_turns
    //run_turns(&d, &monster);

    // kill any monsters in the spot moved to
    combat(&d, &monster);
    //set monster on the map as the monster currently there
    d->mons[monster->position[dim_y]][monster->position[dim_y]] = monster->sequence;
}

void combat(dungeon_t *d, monster_t *m) {
  int x = m->position[dim_x];
  int y = m->position[dim_y];

  // if the if monster next pos has a monster in that spot
  if (d->mons[y][x] != ' ') {
    int i = d->mons[y][x];
    monster_t other = d->monsters[i];

    // change that other mons to ded
    other.is_alive = false;

    // make coords -1 -1
    other.position[dim_x] = -1;
    other.position[dim_y] = -1;
  }
}

// cmp for moves priority queue NEED A STRUCT TO ANONYMIZE ALL CHARACTERS FOR SEQU NUMS AND NEXT_TURNS
static int32_t character_cmp(const void *key, const void *with) {
    // if next_turns subtract to zero then return subtracted sequ numbers
    if (((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn == 0)
        return ((monster_t *) key)->sequence - ((monster_t *) with)->sequence;

    return ((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn;
}

// function that puts characters in the priority queue and runs the game until win/lose
void run_turns(dungeon_t *d) {
    //PUTS ALL MONSTERS IN A PRIORITY QUEUE BASED OFF OF THE CMP
    heap_t h;
    static mp_t *mon, *c;
    int i, j, xpos, ypos, size;
    static uint32_t initialized = 0;

    if (!initialized) {
      initialized = 1;
      for (i = 0; i < d->num_monsters; i++) {
        mon[i].pos[0] = d->monsters[i].position[dim_y];
        mon[i].pos[1] = d->monsters[i].position[dim_x];
      }
    }

    //might need to put before generate monsters or in main
    heap_init(&h, character_cmp, NULL);

    //INITIALIZE BY PUTTING THE MONSTERS IN THE HEAP
    for (i = 0; i < d->num_monsters; i++) {
        mon[i].hn = heap_insert(&h, &mon[i]);
    }
    size = h.size;

    //WHILE THE GAME HASNT BEEN WON
    while (!game_done(&d)) {
      if (--size != h.size) {
        exit(1);
      }
        //TAKE THE TOP MONSTER OUT OF THE QUEUE AND MOVE IT
        c = heap_remove_min(&h);
        ypos = (int) c->pos[0];
        xpos = (int) c->pos[1];
        j = d->mons[ypos][xpos];


        move(&(d->monsters[j]), &d, &h);

        //UPDATE NEXTTURNS
        d->monsters[j].next_turn = d->monsters[j].next_turn + (1000 / d->monsters[j].speed);

        // PUT BACK IN THE HEAP IF IT IS STILL ALIVE
        if (d->monsters[j].is_alive) {
            mon[j].hn = heap_insert(&h, &mon[j]);
        }
        else {
          heap_decrease_key_no_replace(&h, &mon[j].hn);
          c->hn = NULL;
        }

        //RENDER DUNGEON
        render_dungeon(&d);
    }
}
