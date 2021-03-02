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
    // set speed
    d.monsters[i].speed = (rand() % 20) + 5;
  }
}

// boolean decides if the game has been won or lost
bool game_done(dungeon_t *d) {
    int i;
    //game ends if pc is dead 
    if(d.pc.alive) return false;
    // or if all monsters are dead
    for (i = 0; i < d->num_monsters; i++) {
      if (d.monsters[i].alive) return false;
    }
    return true;
}

//dumb monsters that go straight (Unintelligent)
void dumb_monsters(monster_t *monster, dungeon_t *d){
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


//for non-telepathic monsters
void no_line_of_sight() {
  
}


void dijkstra_monster(monster_t *monster, dungeon_t *d) {
    // implementing dijkstra for tuneeling and non tunneling monsters 

    // monster x, y positions 
    monster_X = pos[1];
    monster_Y = pos[0];
    int x = 0;
    int y = 0;
    
    if (/*monsters can tunnel*/) {
        
      
    } else {
      //find shortest path for non tunnel monsters 
    }
}


// moves the monsters for all possibilities 
void move(monster_t *monster, dungeon_t *d, heap_t *heap) {
  // uses dijkstra_monster() to find path for the monsters 
  
  if(monster.type->'@') {
    // pc moves randomly
    
  } else if(monster.type == '1') {
    // monster will move if it sees pc
    
  } else if(monster.type == '2') {
    //monster moves if it sees pc or eradically

  } else if(monster.type == '3') {
    // monster can tunnel (moves eradically or can see pc)

  } else if(monster.type == '4') {
    // monster can see pc and goes in straght line 

  } else if(monster.type == '5') {
    // monster moves eradically, knows pc location and moves in straight line 

  } else if(monster.type == '6') {
    // tunnel monster moves straight towrads pc

  } else if(monster.type == '7') {
    // tunnel monster moves straight towrads pc or moves eradically 

  } else if(monster.type == '8') {
    // tunnel monster moves straight towrads pc or moves eradically 

  } else if(monster.type == '9') {
    

  } else if(monster.type == 'a') {
    

  } else if(monster.type == 'b') {
    

  } else if(monster.type == 'c') {
    

  } else if(monster.type == 'd') {
    

  } else if(monster.type == 'e') {

    
  } else if(monster.type == 'f') {
    

  } else {
    

  }
   // monsters that can turn??

}

int main(int argc, char *argv[]) {



}
