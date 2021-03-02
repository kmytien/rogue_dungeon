//include statements below
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dungeon.h"
#include "path.h"


/**
    Notes:
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
      -basically whats supposed to happen in main(?) is
        - all monsters move (based on type) towards a (stationary) pc
        - when 2 characters land on the same space, the newcomer kills the original player on the cell
        - the game continues until the player dies or all the monsters are dead
        - (use usleep(3) which sleeps for anargument number of microseconds (250000ish) and when the game ends, prints win/lose status before exiting)
**/


 // how do we figure out the number of monsters we need? (argc)
void generate_monsters(dungeon_t *d, char m){ // REMOVED NUM_MONSTERS AND MADE AN INT IN THE DUNGEON STRUCT -H
  int i, j, type[4], x, y;
  bool valid = false;

  // generate needed amnt of monsters
  for(i = 0; i < d->num_monsters; i++) {
    // find monster position that is on floor and is not the pc and place it there
    while (!valid) {
        x = (rand() % 80) + 1;
        y = (rand() % 20) + 1;

        if (d->map[y][x] == ter_floor && d.pc.position[dim_x] != x && d.pc.position[dim_y] != y) {
            valid = true;
            d.monsters[i].postion[dim_x] = x;
            d.monsters[i].postion[dim_y] = y;
        }
    }

    // generate hexidecimal monster type and set as char m
    // do we need srand(time(NULL))?
    int mon = rand() % 15;
    if (mon <= 9)
        d.monsters[i].m = '' + mon;

    else {
        if (mon == 10) d.monsters[i].m = 'a';
        else if (mon == 11) d.monsters[i].m = 'b';
        else if (mon == 12) d.monsters[i].m = 'c';
        else if (mon == 13) d.monsters[i].m = 'd';
        else if (mon == 14) d.monsters[i].m = 'e';
        else d.monsters[i].m = 'f';
    }

    // convert to binary
    for(j = 0; mon > 0; j++) { // type is stored in an array of 4 numbers
        type[v] = mon % 2;
        mon /= 2;
    }

    // set struct levels
    d.monsters[i].mon_type[0] = type[0]; // sets erratic behavior
    d.monsters[i].mon_type[1] = type[1]; // sets tunneling
    d.monsters[i].mon_type[2] = type[2]; // sets telepathy
    d.monsters[i].mon_type[3] = type[3]; // sets intelligence
    d.monsters[i].speed = (rand() % 20) + 5; //set speed
  }
}

// boolean decides if the game has been won or lost
bool game_done(dungeon_t *d) {
    int i;
    //game ends if pc is dead
    if(d.pc.alive)
        return false;

    //or if all monsters are dead
    for (i = 0; i < d->num_monsters; i++) {
        if (d.monsters[i].alive)
            return false;
    }

    return true;
}

//for unintelligent monsters (to move straight)
void straight(monster_t *monster, dungeon_t *d){
    if(monster->pos[1] < d.pc.position[dim_x]) {
        monster->next_pos[0] = monster->pos[0];
        monster->next_pos[1] = monster->pos[1] + 1;
    }

    else if(monster->pos[1] > d.pc.position[dim_x]) {
        monster->next_pos[0] = monster->pos[0];
        monster->next_pos[1] = monster->pos[1] - 1;
    }

    else if(monster->pos[0] < d.pc.position[dim_y]) {
        monster->next_pos[0] = monster->pos[0] + 1;
        monster->next_pos[1] = monster->pos[1];
    }

    else if(monster->pos[0] > d.pc.position[dim_y]) {
        monster->next_pos[0] = monster->pos[0] - 1;
        monster->next_pos[1] = monster->pos[1];
    }
}


//for non telepathic & non tunneling monsters if they see pc in line of sight
//so most likely if they're in a room
void line_of_sight(dungeon_t *d, int monster_y, int monster_x) {
    int xdir, ydir
    bool y_negative = false, x_negative = false; //if pc is left/above monster or right/below monster

    if (monster_x > d->pc.position[dim_x]) {
        x_negative = true;
        x_dir = monster_x - d->pc.position[dim_x];
    }

    else x_dir = d->pc.position[dim_x] - monster_x;

    if (monster_y > d->pc.position[dim_y]) {
        y_negative = true;
        y_dir = monster_y = d->pc.position[dim_y];
    }

    else y_dir = d->pc.position[dim_y] - monster_y;

    //finishing later -mk
    if (!x_negative) {

    }

    else {

    }

    if (!y_negative) {

    }

    else {

    }
}

//implementing dijkstra for tuneeling and non tunneling monsters
void shortest_path(monster_t *monster, dungeon_t *d) {
    //need to use sheaffer's priority queue - i can look into that - mk

    //monster x, y positions
    int x = 0, y = 0;
    int monster_X = monster.pos[1], monster_Y = monster.pos[0];

    //need to use pc_tunnel[][] that sheaffer used 
    if (/*monsters can tunnel*/) {
        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

    }

    //need to use pc_distance[][] that sheaffer used
    else if (/*monsters can't tunnel*/) {
        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }

        if () {

        }
    }
}


// moves the monsters for all possibilities
void move(monster_t *monster, dungeon_t *d, heap_t *heap) {
  // uses dijkstra_monster() to find path for the monsters

  //player character
  if(monster->type == '@') {
    //are we going to let this stay still?
    //we can prob make it move around at least one block :P if we have time

    //not erratic, not tunneling, not telepathic, not smart
  } else if(monster.type == '0') {
    //if pc is in line of sight -> move towards pc

    //not erratic, not tunneling, not telepathic, smart
  } else if(monster.type == '1') {
    // monster will move if it sees pc

    //not erratic, not tunneling, telepathic, not smart
  } else if(monster.type == '2') {
    //monster moves if it sees pc or eradically

    //not erratic, not tunneling, telepathic, smart
  } else if(monster.type == '3') {
    // monster can tunnel (moves eradically or can see pc)

    //not erratic, tunneling, not telepathic, not smart
  } else if(monster.type == '4') {
    // monster can see pc and goes in straght line

    //not erratic, tunneling, not telepathic, smart
  } else if(monster.type == '5') {
    // monster moves eradically, knows pc location and moves in straight line

    //not erratic, tunneling, telepathic, not smart
  } else if(monster.type == '6') {
    // tunnel monster moves straight towrads pc

    //not erratic, tunneling, telepathic, smart
  } else if(monster.type == '7') {
    // tunnel monster moves straight towrads pc or moves eradically

    //erratic, not tunneling, not telepathic, not smart
  } else if(monster.type == '8') {
    // tunnel monster moves straight towrads pc or moves eradically

    //erratic, not tunneling, not telepathic, smart
  } else if(monster.type == '9') {


    //erratic, not tunneling, telepathic, not smart
  } else if(monster.type == 'a') {


    //erratic, not tunneling, telepathic, smart
  } else if(monster.type == 'b') {


    //erratic, tunneling, not telepathic, not smart
  } else if(monster.type == 'c') {


    //erratic, tunneling, not telepathic, smart
  } else if(monster.type == 'd') {


    //erratic, tunneling, telepathic, not smart
  } else if(monster.type == 'e') {


    //erratic, tunneling, telepathic, smart (f)
  } else {


  }
   // monsters that can turn??

}

int main(int argc, char *argv[]) {



}
