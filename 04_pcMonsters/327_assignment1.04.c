#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "dungeon.h"
#include "dims.h"
#include "heap.h"
#include "path.h"
#include "assignment_104.h"


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


//generates all monsters in dungeon
void generate_monsters(dungeon_t *d) {
    int i, j, type[4], x, y, seq;
    int num = d->num_monsters;
    bool valid = false;
    d->monsters = malloc(d->num_monsters * sizeof(int));

    // init mons arrays
    for (y = 1; y < 21; y++) {
        for (x = 1; x < 80; x++) {
            d->mons[y][x] = 0; // int array, might need to be changed
        }
    }

    // generate needed amnt of monsters
    for (i = 0; i < num; i++) {

        // find monster position that is on floor and is not the pc and place it there
        while (!valid) {
            x = (rand() % 79) + 1;
            y = (rand() % 19) + 1;

            if (d->map[y][x] >= ter_floor && d->pc.position[dim_x] != x && d->pc.position[dim_y] != y) {
                valid = true;
                d->monsters[i].position[dim_x] = x;
                d->monsters[i].position[dim_y] = y;

                // printf("\n%d ", x);
                // printf("%d ", y);
            }
        }

        // generate hexidecimal monster type and set as char m
        // do we need srand(time(NULL))?
        int mon = rand() % 16;

        switch(mon) {
            case 0:
            	d->monsters[i].m = '0';
            	break;
            case 1:
            	d->monsters[i].m = '1';
            	break;
            case 2:
            	d->monsters[i].m = '2';
            	break;
            case 3:
            	d->monsters[i].m = '3';
            	break;
            case 4:
            	d->monsters[i].m = '4';
            	break;
            case 5:
            	d->monsters[i].m = '5';
            	break;
            case 6:
            	d->monsters[i].m = '6';
            	break;
            case 7:
            	d->monsters[i].m = '7';
            	break;
            case 8:
            	d->monsters[i].m = '8';
            	break;
            case 9:
            	d->monsters[i].m = '9';
            	break;
            case 10:
            	d->monsters[i].m = 'a';
            	break;
            case 11:
            	d->monsters[i].m = 'b';
            	break;
            case 12:
            	d->monsters[i].m = 'c';
            	break;
            case 13:
            	d->monsters[i].m = 'd';
            	break;
            case 14:
            	d->monsters[i].m = 'e';
            	break;
            case 15:
            	d->monsters[i].m = 'f';
            	break;
        }
        seq = i;
        seq++;
        d->mons[d->monsters[i].position[dim_y]][d->monsters[i].position[dim_x]] = seq;

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
            d->monsters[i].last_pos[0] = 0;
            d->monsters[i].last_pos[1] = 0;
        }

        // set sequence
        d->monsters[i].sequence = seq;

        // set next_turns
        d->monsters[i].next_turn = 0;
        d->monsters[i].is_alive = true;

        valid = false;

        //print central
        // printf(" %c hex, %d xpos, %d ypos, bin %d%d%d%d, next turns %d, seq %d", d->monsters[i].m, d->monsters[i].position[dim_x], d->monsters[i].position[dim_y], 
        //         d->monsters[i].mon_type[0], d->monsters[i].mon_type[1], d->monsters[i].mon_type[2], d->monsters[i].mon_type[3], d->monsters[i].next_turn,
        //         d->monsters[i].sequence);
    }
    // printf("end of gen");
}

//boolean decides if the game has been won or lost and prints if the game is done
bool game_done(dungeon_t *d) {
    int i;

    //game ends if pc is dead
    if (!d->pc.is_alive) {
        printf("%s\n", lose_message);
        return true;
    }

    //or if all monsters are dead
    for (i = 0; i < d->num_monsters; i++) {
        if (d->monsters[i].is_alive)
            return false;
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


    if (y_dir > x_dir) {
        i1 = x_dir * 2;
        i2 = i1 - y_dir;
        i3 = i2 - y_dir;

        for (int i = 0; i < y_dir + 1; i++) {
            //checks through y direction if all spots isn't a floor tile (rock -> NOT in line of sight)
            if ((d->map[monster_y][monster_x] < ter_floor) && (i != y_dir))
                return 0;

            //calculating coords for next pixel
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
            if ((d->map[monster_y][monster_x] != ter_floor) && (i != x_dir))
                return 0;

            //calculating coords for next pixel
            if (i2 < 0) i2 += i1;
            else {
                i2 += i3;
                monster_y++;
            }
        }
        return 1;
    }
}


//implementation for tunneling and non tunneling monsters
void shortest_path(monster_t* monster, dungeon_t* d) {

    int x = 0, y = 0, cost = INT_MAX;
    int monster_x = monster->next_pos[1], monster_y = monster->next_pos[0];
    int next_x, next_y;
    bool tunneling;

    //looking for if monster is tunneling or not
    switch (monster->m) {
        case '4':
        case '5':
        case '6':
        case '7':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            tunneling = true;
            break;

        default:
            tunneling = false;
    }


    if (tunneling) {

        if (cost > (d->pc_tunnel[monster_y - 1][monster_x] + d->hardness[monster_y - 1][monster_x] / 85)) {
            cost = (d->pc_tunnel[monster_y - 1][monster_x] + d->hardness[monster_y - 1][monster_x] / 85);
            next_x = monster_x;
            next_y = monster_y - 1;
        }

        if (cost > (d->pc_tunnel[monster_y + 1][monster_x] + d->hardness[monster_y + 1][monster_x] / 85)) {
            cost = (d->pc_tunnel[monster_y + 1][monster_x] + d->hardness[monster_y + 1][monster_x] / 85);
            next_x = monster_x;
            next_y = monster_y + 1;
        }

        if (cost > (d->pc_tunnel[monster_y][monster_x - 1] + d->hardness[monster_y][monster_x - 1] / 85)) {
            cost = (d->pc_tunnel[monster_y][monster_x - 1] + d->hardness[monster_y][monster_x - 1] / 85);
            next_x = monster_x - 1;
            next_y = monster_y;
        }

        if (cost > (d->pc_tunnel[monster_y][monster_x + 1] + d->hardness[monster_y][monster_x + 1] / 85)) {
            cost = (d->pc_tunnel[monster_y][monster_x + 1] + d->hardness[monster_y][monster_x + 1] / 85);
            next_x = monster_x + 1;
            next_y = monster_y;
        }

        if (cost > (d->pc_tunnel[monster_y - 1][monster_x - 1] + d->hardness[monster_y - 1][monster_x - 1] / 85)) {
            cost = (d->pc_tunnel[monster_y - 1][monster_x - 1] + d->hardness[monster_y - 1][monster_x - 1] / 85);
            next_x = monster_x - 1;
            next_y = monster_y - 1;
        }

        if (cost > (d->pc_tunnel[monster_y + 1][monster_x + 1] + d->hardness[monster_y + 1][monster_x + 1] / 85)) {
            cost = (d->pc_tunnel[monster_y + 1][monster_x + 1] + d->hardness[monster_y + 1][monster_x + 1] / 85);
            next_x = monster_x + 1;
            next_y = monster_y + 1;
        }

        if (cost > (d->pc_tunnel[monster_y + 1][monster_x - 1] + d->hardness[monster_y + 1][monster_x - 1] / 85)) {
            cost = (d->pc_tunnel[monster_y + 1][monster_x - 1] + d->hardness[monster_y + 1][monster_x - 1] / 85);
            next_x = monster_x - 1;
            next_y = monster_y + 1;
        }

        if (cost > (d->pc_tunnel[monster_y - 1][monster_x + 1] + d->hardness[monster_y - 1][monster_x + 1] / 85)) {
            cost = (d->pc_tunnel[monster_y - 1][monster_x + 1] + d->hardness[monster_y - 1][monster_x + 1] / 85);
            next_x = monster_x + 1;
            next_y = monster_y - 1;
        }

        //checks hardness and changes based on rules
        if (hardnessxy(x, y) <= 85) {
            monster->next_pos[0] = next_y;
            monster->next_pos[1] = next_x;
        }

        tunneling_hardness(d, next_x, next_y);
    }

    //nontunneling monsters
    else {
        if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y - 1][monster_x]) {
            monster->next_pos[0]--;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y + 1][monster_x]) {
            monster->next_pos[0]++;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y][monster_x - 1]) {
            monster->next_pos[1]--;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y][monster_x + 1]) {
            monster->next_pos[1]++;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y - 1][monster_x - 1]) {
            monster->next_pos[0]--;
            monster->next_pos[1]--;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y + 1][monster_x + 1]) {
            monster->next_pos[0]++;
            monster->next_pos[1]++;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y + 1][monster_x - 1]) {
            monster->next_pos[0]++;
            monster->next_pos[1]--;
        }

        else if (d->pc_distance[monster_y][monster_x] > d->pc_distance[monster_y - 1][monster_x + 1]) {
            monster->next_pos[0]--;
            monster->next_pos[1]++;
        }
    }
}


//changes hardness and updates maps
void tunneling_hardness(dungeon_t* d, int x, int y) {
    if (hardnessxy(x, y) <= 85 && hardnessxy(x, y) != 0) {
        hardnessxy(x, y) = 0;
        d->map[x][y] = ter_floor_hall;

        dijkstra_tunnel(d);
        dijkstra(d);
    }

    else
        hardnessxy(x, y) -= 85;
}


//moves the monsters for all possibilities
void move(monster_t *monster, dungeon_t *d, heap_t *heap) {

    //remove monster from monster map
    d->mons[monster->position[dim_y]][monster->position[dim_x]] = ' ';

    //pc
    if (monster->m == '@') {

        monster->next_pos[dim_y] = monster->position[dim_y] + d->pc.position[dim_y];
        monster->next_pos[dim_x] = monster->position[dim_x]+ d->pc.position[dim_x];


      //not erratic, not tunneling, not telepathic, not smart
    } else if (monster->m == '0') {

        if (in_line_of_sight(d, monster))
            straight(monster, d);

        else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //not erratic, not tunneling, not telepathic, smart
    } else if (monster->m == '1') {

        if (in_line_of_sight(d, monster))
            straight(monster, d);

        else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //not erratic, not tunneling, telepathic, not smart
    } else if (monster->m == '2') {

        if (in_line_of_sight(d, monster))
            straight(monster, d);

        else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //not erratic, not tunneling, telepathic, smart
    } else if (monster->m == '3') {

        shortest_path(monster, d);


      //not erratic, tunneling, not telepathic, not smart
    } else if (monster->m == '4') {

        if (in_line_of_sight(d, monster))
            straight(monster, d);

        else {
            int rand_x = (rand() % 2) - 1;
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

        if (in_line_of_sight(d, monster))
            straight(monster, d);

        else {
            int rand_x = (rand() % 2) - 1;
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

        int rand_x = (rand() % 2) - 1;
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

        shortest_path(monster, d);


      //erratic, not tunneling, not telepathic, not smart
    } else if (monster->m == '8') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else if (in_line_of_sight(d, monster)) {
            straight(monster, d);

        } else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //erratic, not tunneling, not telepathic, smart
    } else if (monster->m == '9') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else if (in_line_of_sight(d, monster)) {
            straight(monster, d);

        } else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //erratic, not tunneling, telepathic, not smart
    } else if (monster->m == 'a') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else if (in_line_of_sight(d, monster)) {
            straight(monster, d);

        } else {
            monster->next_pos[dim_y] = monster->position[dim_y];
            monster->next_pos[dim_x] = monster->position[dim_x];
        }


      //erratic, not tunneling, telepathic, smart
    } else if (monster->m == 'b') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else {
            shortest_path(monster, d);
        }


      //erratic, tunneling, not telepathic, not smart
    } else if (monster->m == 'c') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else if (in_line_of_sight(d, monster)) {
            straight(monster, d);

        } else {
            int rand_x = (rand() % 2) - 1;
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
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else if (in_line_of_sight(d, monster)) {
            straight(monster, d);

        } else {
            int rand_x = (rand() % 2) - 1;
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
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else {
            int rand_x = (rand() % 2) - 1;
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


      //erratic, tunneling, telepathic, smart (f)
    } else if (monster->m == 'f') {
        int is_erratic = rand() % 2;

        if (is_erratic) {
            int rand_x = (rand() % 2) - 1;
            int rand_y = (rand() % 2) - 1;

            monster->next_pos[dim_y] = monster->position[dim_y] + rand_x;
            monster->next_pos[dim_x] = monster->position[dim_x] + rand_y;

        } else {
            shortest_path(monster, d);
        }
    }

    //kill any monsters in the spot moved to
    combat(d, monster);

    //set monster on the map as the monster currently there
    d->mons[monster->position[dim_y]][monster->position[dim_y]] = monster->sequence;
}


//functions for killing monsters/pc
void combat(dungeon_t *d, monster_t *m) {

    int x = m->position[dim_x];
    int y = m->position[dim_y];

    if (d->pc.position[dim_y] == y && d->pc.position[dim_x] == x) {
      d->pc.is_alive == false;
      d->pc.position[dim_y] == -1;
      d->pc.position[dim_x] == -1;
    }

    //if the if monster next pos has a monster in that spot
    if (d->mons[y][x] != ' ') {
        int i = d->mons[y][x];
        monster_t other = d->monsters[i];

        //change that other mons to ded
        other.is_alive = false;

        //make coords -1 -1
        other.position[dim_x] = -1;
        other.position[dim_y] = -1;
    }
}


//cmp for moves priority queue NEED A STRUCT TO ANONYMIZE ALL CHARACTERS FOR SEQU NUMS AND NEXT_TURNS
static int32_t character_cmp(const void *key, const void *with) {
    // if next_turns subtract to zero then return subtracted sequ numbers
    if (((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn == 0)
        return ((monster_t *) key)->sequence - ((monster_t *) with)->sequence;

    return ((monster_t *) key)->next_turn - ((monster_t *) with)->next_turn;
}


//function that puts characters in the priority queue and runs the game until win/lose
void run_turns(dungeon_t *d) {
    //PUTS ALL MONSTERS IN A PRIORITY QUEUE BASED OFF OF THE CMP
    heap_t h;
    static mp_t *mon, *c;
    int i, j, xpos, ypos, size;
    static uint32_t initialized = 0;

    mon = malloc(d->num_monsters * sizeof(int));

    generate_monsters(d);
    d->pc.is_alive = true;

    // for (int k = 0; k < 10; k++) {
    //     printf("%d ", d->monsters[k].position[dim_x]);
    //     printf("%d\n", d->monsters[k].position[dim_y]);
    // }
// printf("\n");
//     for (int y = 0; y < DUNGEON_Y; y++) {
//         for (int x = 0; x < DUNGEON_X; x++) {
//             printf("%d", d->mons[y][x]);
//         }
//         printf("\n");
//     }

    // render_dungeon(d);

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
    while (!game_done(d)) {
        // if (--size != h.size) {
        //     exit(1);
        // }

        //TAKE THE TOP MONSTER OUT OF THE QUEUE AND MOVE IT
        c = heap_remove_min(&h);
        ypos = (int) c->pos[0];
        xpos = (int) c->pos[1];
        j = d->mons[ypos][xpos];
        j--;

        //UPDATE NEXTTURNS

        // PUT BACK IN THE HEAP IF IT IS STILL ALIVE
        if (d->monsters[j].is_alive) {
            mon[j].hn = heap_insert(&h, &mon[j]);
            move(&(d->monsters[j]), d, &h);
            d->monsters[j].next_turn += (1000 / d->monsters[j].speed);
        }
        else {
            heap_decrease_key_no_replace(&h, mon[j].hn);
            c->hn = NULL;
        }
        //RENDER DUNGEON
        usleep(250000);
        render_dungeon(d);
    }

    if (d->pc.is_alive)
    	printf("%s\n", win_message);

    free(d->monsters);

}
