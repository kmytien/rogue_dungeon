//include statements below
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dungeon.h"
#include "path.h"

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

struct tunnel(){
   heap_node_t *hn;
   uint8_t pos[2];
   int32_t cost;
} tunnel_t;

struct nontunnel(){
   heap_node_t *hn;
   uint8_t pos[2];
   int32_t cost;
} nontunnel_t;


struct player {

} pc;

struct nonplayer {

} npc;

struct character {
    int x_pos, y_pos;
    int speed;
    int next_turn;

    union {
        struct player;
        struct nonplayer;
        int is_alive;
    }
};

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

        // set sequence
        d.monsters[i].sequence = i+1;

        // set next_turns
        d.monsters[i].next_turn = 0;
    }
}

//boolean decides if the game has been won or lost and prints if the game is done
bool game_done(dungeon_t *d) {
    int i;
    //game ends if pc is dead
    if(d.pc.alive) {
      printf(lose_message);
      return false;
    }

    //or if all monsters are dead
    for (i = 0; i < d->num_monsters; i++) {
        if (d.monsters[i].alive) {
            printf(win_message);
            return false;
        }
    }

    return true;
}

//for unintelligent monsters (to move straight)
void straight(monster_t *monster, dungeon_t *d){
    if(monster->position[dim_x] < d.pc.position[dim_x]) {
        monster->next_pos[dim_y] = monster->posi;
        monster->next_pos[dim_x] = monster->position[1] + 1;
    }

    else if(monster->position[dim_x] > d.pc.position[dim_x]) {
        monster->next_pos[dim_y] = monster->position[dim_y];
        monster->next_pos[dim_x] = monster->position[dim_x] - 1;
    }

    else if(monster->position[dim_y] < d.pc.position[dim_y]) {
        monster->next_pos[dim_y] = monster->position[dim_y] + 1;
        monster->next_pos[dim_x] = monster->position[dim_x];
    }

    else if(monster->position[dim_y] > d.pc.position[dim_y]) {
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
    int xdir, ydir, i1, i2, i3;

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
    }

    else {
        i1 = y_dir * 2;
        i2 = i1 - x_dir;
        i3 = i2 - x_dir;

        for (int i = 0; i < x_dir + 1; i++) {
            //checks through y direction if all spots isn't a floor tile (rock -> NOT in line of sight)
            if ((d->map[monster_y][monster_x] < ter_floor) && (i != x_dir))
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

    return 1;
}


//implementing dijkstra for tuneeling and non tunneling monsters
void shortest_path(monster_t *monster, dungeon_t *) {
    //need to use sheaffer's priority queue - i can look into that - mk

    //monster x, y positions
    int x = 0, y = 0, num = INT_MAX;
    int monster_x = monster.pos[1], monster_y = monster.pos[0];

    //need to use pc_tunnel[][] that sheaffer used
    if (tunnel) {
        //need to initialize variables
        //do stuff idk
        //cost > (d->pc_tunnel[monster_y][monster_x] + d->hardness[monster_y][monster_x] / 85)

        //cardinal directions
        if (num > d->dijkstra_tunnel[monster_y - 1][monster_x   ].cost) {
            y = -1;
            num = d->dijkstra_tunnel[monster_y - 1][monster_x   ].cost;
        }

        if (num > d->dijkstra_tunnel[monster_y + 1][monster_x   ]) {
            y = 1;
            num = d->dijkstra_tunnel[monster_y + 1][monster_x   ].cost;
        }

        if (num > d->dijkstra_tunnel[monster_y    ][monster_x - 1] < num) {
            x = -1;
            num = d->dijkstra_tunnel[monster_y    ][monster_x - 1].cost;
        }

        if (num > d->dijkstra_tunnel[monster_y    ][monster_x + 1]) {
            x = 1;
            num = d->dijkstra_tunnel[monster_y    ][monster_x + 1].cost;
        }
    }

    //need to use pc_distance[][] that sheaffer used -- uhh I'm not really sure how to do that lol, lmk if we need to change it tho
    else {
        if (d->dijkstra_nontunnel[monster->pos[0] - 1][monster->pos[1]].cost < num) {
            x = -1;
            num = d->dijkstra_nontunnel[monster->pos[0] - 1][monster->pos[1]].cost;
        }

        if (d->dijkstra_nontunnel[monster->pos[0] + 1][monster->pos[1]].cost < num) {
            x = 1;
            num = d->dijkstra_nontunnel[monster->pos[0] + 1][monster->pos[1]].cost;
        }

        if (d->dijkstra_nontunnel[monster->pos[0]][monster->pos[1] + 1].cost < num) {
            y = 1;
            num = d->dijkstra_nontunnel[monster->pos[0]][monster->pos[1] + 1].cost;
        }

        if (d->dijkstra_nontunnel[monster->pos[0]][monster->pos[1] - 1].cost < num) {
            y = -1;
            num = d->dijkstra_nontunnel[monster->pos[0]][monster->pos[1] - 1].cost;
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
    // uses dijkstra_monster() to find path for the monsters
    //not erratic, not tunneling, not telepathic, not smart
    if(monster.type == '0') {
        //if pc is in line of sight -> move towards pc
        if(in_line_of_sight){
          straight(monster_t *monster, dungeon_t *d);
        }

      //not erratic, not tunneling, not telepathic, smart
    } else if(monster.type == '1') {
        shortest_path(monster_t *monster, dungeon_t *);

      //not erratic, not tunneling, telepathic, not smart
    } else if(monster.type == '2') {
      straight(monster_t *monster, dungeon_t *d);

      //not erratic, not tunneling, telepathic, smart
    } else if(monster.type == '3') {
      shortest_path(monster_t *monster, dungeon_t *);

      //not erratic, tunneling, not telepathic, not smart
    } else if(monster.type == '4') {
      straight(monster_t *monster, dungeon_t *d);

      //not erratic, tunneling, not telepathic, smart
    } else if(monster.type == '5') {
      shortest_path(monster_t *monster, dungeon_t *);

      //not erratic, tunneling, telepathic, not smart
    } else if(monster.type == '6') {
      straight(monster_t *monster, dungeon_t *d);

      //not erratic, tunneling, telepathic, smart
    } else if(monster.type == '7') {
      shortest_path(monster_t *monster, dungeon_t *);

      //erratic, not tunneling, not telepathic, not smart
    } else if(monster.type == '8') {
      straight(monster_t *monster, dungeon_t *d);

      //erratic, not tunneling, not telepathic, smart
    } else if(monster.type == '9') {
      shortest_path(monster_t *monster, dungeon_t *);

      //erratic, not tunneling, telepathic, not smart
    } else if(monster.type == 'a') {
      straight(monster_t *monster, dungeon_t *d);

      //erratic, not tunneling, telepathic, smart
    } else if(monster.type == 'b') {
      shortest_path(monster_t *monster, dungeon_t *);

      //erratic, tunneling, not telepathic, not smart
    } else if(monster.type == 'c') {
      straight(monster_t *monster, dungeon_t *d);

      //erratic, tunneling, not telepathic, smart
    } else if(monster.type == 'd') {
      shortest_path(monster_t *monster, dungeon_t *);

      //erratic, tunneling, telepathic, not smart
    } else if(monster.type == 'e') {
      straight(monster_t *monster, dungeon_t *d);

      //erratic, tunneling, telepathic, smart (f)
    } else {
      shortest_path(monster_t *monster, dungeon_t *);

    }

    //knowing which monster's turn it is
    //something with heap_insert
    run_turns(&d);

}

// cmp for moves priority queue NEED A STRUCT TO ANONYMIZE ALL CHARACTERS FOR SEQU NUMS AND NEXT_TURNS
static int32_t character_cmp(const void *key, const void *with) {
    // if next_turns subtract to zero then return subtracted sequ numbers
    if (((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn == 0)
        return ((monster_t *) key)->sequence - ((monster_t *) with)->sequence;

    return ((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn;
}

// function that puts characters in the priority queue and runs the game until win/lose
void run_turns(dungeon_t *d, monster_t* monster) {
    //PUTS ALL MONSTERS IN A PRIORITY QUEUE BASED OFF OF THE CMP
    heap_t h;
    static path_t monster;
    int i;

    //might need to put before generate monsters or in main
    //heap_init(&h, dist_cmp, NULL); IN MAIN
    
    //INITIALIZE BY PUTTING THE MONSTERS IN THE HEAP
    for (i = 0; i < d->num_monsters; i++) {
        monster.hn = heap_insert(&h, &monster);
    }

    //WHILE THE GAME HASNT BEEN WON
    while(!game_done) {
        //TAKE THE TOP MONSTER OUT OF THE QUEUE AND MOVE IT

        //UPDATE NEXTTURNS
        monster->next_turn = monster->next_turn + (1000 / speed);
        // PUT BACK IN THE HEAP IF IT IS STILL ALIVE
        if (!(d->monster.alive))
            monster.hn = heap_insert(&h, &monster);

        //RENDER DUNGEON
        render_dungeon(d);
    }

}
