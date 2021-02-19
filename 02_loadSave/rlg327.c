#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#define WORLD_ROW 21
#define WORLD_COL 80

void printDungeon();
void initDungeon();
void createRooms();
void staircase();
void spawnPC();
void createCorridors(int);
void setCorridors(int, int, int, int);
bool legality(int, int, int, int);

uint16_t up;
uint16_t down;

//structure for room - represents top left corner of a room
struct room {
    int xstart; //top left row
    int ystart; //top left col
    int xsize; //width
    int ysize; //height'
} *rooms;

//for upstairs
struct upstairs {
    uint8_t up_x;
    uint8_t up_y;
} *ups;

//for downstairs
struct downstairs {
    uint8_t down_x;
    uint8_t down_y;
} *downs;

//for pc
struct player {
    uint8_t row;
    uint8_t col;
} pc;

//for hardness
struct dungeon {
    uint16_t numRooms;
    uint8_t hardness[WORLD_ROW][WORLD_COL];
    char dungeon[WORLD_ROW][WORLD_COL];
} d;

//prints dungeon output
void printDungeon() {
    int row, col;

    for (row = 0; row < WORLD_ROW; row++) {
        for(col = 0; col < WORLD_COL; col++) {

            //border
            if (row == 0 || row == WORLD_ROW - 1)
                printf("-");

            //border
            else if (col == 0 || col == WORLD_COL - 1)
                printf("|");

            //just used character '*' to represent immutable rock (outermost cells)
            else if (d.dungeon[row][col] == '*')
                printf(" ");

            else
                printf("%c", d.dungeon[row][col]);
        }

        printf("\n");
    }
}

//dungeon initialization function
void initDungeon() {

    //make outside edges immutable rock, everything else just rock
    int row, col;
    srand(time(NULL));

    for (row = 0; row < WORLD_ROW; row++) {
        for (col = 0; col < WORLD_COL; col++) {

            //if outermost cell, set as '@', a character representing immutable rock
            if (row > WORLD_ROW - 3 || row < 2 || col > WORLD_COL - 3 || col < 2) {
                d.dungeon[row][col] = '*';
                d.hardness[row][col] = 255;
            }

            else {
                d.dungeon[row][col] = ' ';
                d.hardness[row][col] = (rand() % 254) + 1;
            }
        }
    }
}

//random room generator function
void createRooms() {
    srand(time(NULL));
    //makes at least 6 rooms, with max up to 10
    //x-direction can be 4 to 12 blocks
    //y-direction can be 3 to 9 blocks

    //int maxRooms = 6 + (rand() % 5);
    int num_fails = 0, consec = 0;
    rooms = malloc(20 * sizeof(struct room)); //deleted (int*)
    int currentRooms = 0;

    //keeps adding room until it gets to randomized max num of rooms
    while(num_fails < 100) {

        //getting random room sizes
        int rand_vertical = 3 + (rand() % 7);
        int rand_horizontal = 4 + (rand() % 9);

        //getting random placement
        int row_start = 2 + (rand() % 17);
        int col_start = 2 + (rand() % 77);
        int i, j;

        //legality checks if its legal to place room there
        if (legality(row_start, col_start, rand_vertical, rand_horizontal)) {

            //adds room to dungeon
            for (i = row_start; i < row_start + rand_vertical; i++) {
                for (j = col_start; j < col_start + rand_horizontal; j++) {
                    d.dungeon[i][j] = '.';
                    d.hardness[i][j] = 0;
                }
            }

            //add to the rooms array
            rooms[currentRooms].xstart = row_start;
            rooms[currentRooms].ystart = col_start;
            rooms[currentRooms].xsize = rand_vertical;
            rooms[currentRooms].ysize = rand_horizontal;

            currentRooms++;
            consec = 0;
        }


        else {
            consec = 1;

            if (consec == 1)
                num_fails++;
        }

    }

    d.numRooms = currentRooms;
    //create rooms then corridors
    createCorridors(currentRooms);
}

//checks if placement of room is legal
//if there is a room or if it touches immutable rock then room cannot be placed here
bool legality(int startRow, int startCol, int endRow, int endCol) {
    int i, j;

    for (i = startRow - 1; i < startRow + endRow + 1; i++) {
        for (j = startCol - 1; j < startCol + endCol + 1; j++) {
            if (d.dungeon[i][j] == '.' || d.dungeon[i][j] == '*')
                return false;
        }
    }

    return true;
}

//corridor maker - num is number of rooms that are made
//was int* rooms, int num parameters
void createCorridors(int num) {

    int currRoom;
    struct room firstRoom;
    struct room secondRoom;

    //connects first room to second room, then second to third, third to fourth etc..
    for (currRoom = 0; currRoom < num; currRoom++) {
        firstRoom.xstart = rooms[currRoom].xstart;
        firstRoom.ystart = rooms[currRoom].ystart;
        firstRoom.xsize = rooms[currRoom].xsize;
        firstRoom.ysize = rooms[currRoom].ysize;

        //set for secondRoom struct
        //if at last room, connect last room to first room in array
        if (currRoom == num - 1) {
            secondRoom.xstart = rooms[0].xstart;
            secondRoom.ystart = rooms[0].ystart;
            secondRoom.xsize = rooms[0].xsize;
            secondRoom.ysize = rooms[0].ysize;
        }

        else {
            secondRoom.xstart = rooms[currRoom + 1].xstart;
            secondRoom.ystart = rooms[currRoom + 1].ystart;
            secondRoom.xsize = rooms[currRoom + 1].xsize;
            secondRoom.ysize = rooms[currRoom + 1].ysize;
        }

        //actually sets corridors it in dungeon
        setCorridors(firstRoom.xstart, firstRoom.ystart, secondRoom.xstart, secondRoom.ystart);
    }

    //placing staircases
    staircase();
}

//sets corridors in map
void setCorridors(int fX, int fY, int eX, int eY) {

    //starting at rows
    if (fX < eX) {
        for (int i = fX; i < eX; i++) {
            if (d.dungeon[i][fY] == ' ') {
                d.dungeon[i][fY] = '#';
                d.hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (d.dungeon[eX][i] == ' ') {
                    d.dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (d.dungeon[eX][i] == ' ') {
                    d.dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }
    }


    else {
        for (int i = fX; i > eX; i--) {
            if (d.dungeon[i][fY] == ' ') {
                d.dungeon[i][fY] = '#';
                d.hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (d.dungeon[eX][i] == ' ') {
                    d.dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (d.dungeon[eX][i] == ' ') {
                    d.dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }
    }
}

//up staircase with '<' and down staircase with '>'
void staircase() {

    int i;
    bool isFloor = false;

    //randomly generate num between 1 and 2 and then place 1-2 of each staircase in a room
    //up stairs '<'
    up = (rand() % 2) + 1;
    ups = malloc(sizeof(struct upstairs) * 2);
    for (i = 0; i < up; i++) {

        //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            ups[i].up_x = (rand() % 79) + 1; //col
            ups[i].up_y = (rand() % 20) + 1; //row

            //[up.up_y][up.up_x] == [row][col]
            if (d.dungeon[ups[i].up_y][ups[i].up_x] == '.') {
                d.dungeon[ups[i].up_y][ups[i].up_x] = '<';
                d.hardness[ups[i].up_y][ups[i].up_x] = 0;
                isFloor = true;
            }
        }

        isFloor = false;
    }

    //down stairs '>'
    down = (rand() % 2) + 1;
    downs = malloc(sizeof(struct downstairs) * 2);
    for (i = 0; i < down; i++) {

        //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            downs[i].down_x = (rand() % 79) + 1;
            downs[i].down_y = (rand() % 20) + 1;

            if (d.dungeon[downs[i].down_y][downs[i].down_x] == '.') {
                d.dungeon[downs[i].down_y][downs[i].down_x] = '>';
                d.hardness[downs[i].down_y][downs[i].down_x] = 0;
                isFloor = true;
            }
        }

        isFloor = false;
    }

    spawnPC();
}

void spawnPC () {
    int x, y;
    bool spawn = false;
    srand(time(NULL));

    while (!spawn) {
        x = (rand() % 79) + 1;
        y = (rand() % 20) + 1;

        if (d.dungeon[y][x] == '.') {
            pc.col = x;
            pc.row = y;
            spawn = true;
        }
    }
}
