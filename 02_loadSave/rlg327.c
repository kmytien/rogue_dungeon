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
void createCorridors(int*,int);
void setCorridors(int, int, int, int);
bool legality(int, int, int, int);

char dungeon[WORLD_ROW][WORLD_COL];
int hardness[WORLD_ROW][WORLD_COL];
int* roomsDimensions;
uint16_t up;
uint16_t down;
uint16_t numRooms;

//structure for room - represents top left corner of a room
struct room {

    int xstart; //top left row
    int ystart; //top left col
    int xsize; //width
    int ysize; //height
};

//for upstairs
struct upstairs {
    uint8_t up_x;
    uint8_t up_y;
} *upS[4];

//for downstairs
struct downstairs {
    uint8_t down_x;
    uint8_t down_y;
} *downS[4];

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
            else if (dungeon[row][col] == '*')
                printf(" ");

            else
                printf("%c", dungeon[row][col]);
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
                dungeon[row][col] = '*';
                hardness[row][col] = 255;
            }

            else {
                dungeon[row][col] = ' ';
                hardness[row][col] = (rand() % 254) + 1;
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
    int* rooms = (int*) malloc(20 * 4 * sizeof(int));
    roomsDimensions = (int*) malloc(20 * 4 * sizeof(int));
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
                    dungeon[i][j] = '.';
                    hardness[i][j] = 0;
                }
            }

            //add to the rooms array
            rooms[4 * currentRooms] = row_start;
            rooms[(4 * currentRooms) + 1] = col_start;
            rooms[(4 * currentRooms) + 2] = rand_vertical;
            rooms[(4 * currentRooms) + 3] = rand_horizontal;

            //for global var to use in 1.02
            roomsDimensions[4 * currentRooms] = row_start;
            roomsDimensions[(4 * currentRooms) + 1] = col_start;
            roomsDimensions[(4 * currentRooms) + 2] = rand_vertical;
            roomsDimensions[(4 * currentRooms) + 3] = rand_horizontal;

            currentRooms++;
            consec = 0;
        }

        
        else {
            consec = 1;

            if (consec == 1)
                num_fails++;
        }
        
    }

    //create rooms then corridors
    createCorridors(rooms, currentRooms);
}

//checks if placement of room is legal
//if there is a room or if it touches immutable rock then room cannot be placed here
bool legality(int startRow, int startCol, int endRow, int endCol) {
    int i, j;

    for (i = startRow - 1; i < startRow + endRow + 1; i++) {
        for (j = startCol - 1; j < startCol + endCol + 1; j++) {
            if (dungeon[i][j] == '.' || dungeon[i][j] == '*')
                return false;
        }
    }

    return true;
}

//corridor maker - num is number of rooms that are made
//was int* rooms, int num parameters
void createCorridors(int* rooms, int num) {

    int currRoom;
    struct room firstRoom;
    struct room secondRoom;

    //connects first room to second room, then second to third, third to fourth etc..
    for (currRoom = 0; currRoom < num; currRoom++) {
        firstRoom.xstart = rooms[4 * currRoom];
        firstRoom.ystart = rooms[(4 * currRoom) + 1];
        firstRoom.xsize = rooms[(4 * currRoom) + 2];
        firstRoom.ysize = rooms[(4 * currRoom) + 3];

        //set for secondRoom struct
        //if at last room, connect last room to first room in array
        if (currRoom == num - 1) {
            secondRoom.xstart = rooms[0];
            secondRoom.ystart = rooms[1];
            secondRoom.xsize = rooms[2];
            secondRoom.ysize = rooms[3];
        }

        else {
            secondRoom.xstart = rooms[(4 * currRoom) + 4];
            secondRoom.ystart = rooms[(4 * currRoom + 1) + 4];
            secondRoom.xsize = rooms[(4 * currRoom + 2) + 4];
            secondRoom.ysize = rooms[(4 * currRoom + 3) + 4];
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
            if (dungeon[i][fY] == ' ') {
                dungeon[i][fY] = '#';
                hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    hardness[eX][i] = 0;
                }
            }
        }
    }


    else {
        for (int i = fX; i > eX; i--) {
            if (dungeon[i][fY] == ' ') {
                dungeon[i][fY] = '#';
                hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    hardness[eX][i] = 0;
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
    struct upstairs ups[up];
    for (i = 0; i < up; i++) {

        //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            ups[i].up_x = (rand() % 79) + 1; //col
            ups[i].up_y = (rand() % 20) + 1; //row

            //[up.up_y][up.up_x] == [row][col]
            if (dungeon[ups[i].up_y][ups[i].up_x] == '.') {
                dungeon[ups[i].up_y][ups[i].up_x] = '<';
                hardness[ups[i].up_y][ups[i].up_x] = 0;
                isFloor = true;
            }
        }

        isFloor = false;
    }

    //down stairs '>'
    down = (rand() % 2) + 1;
    struct downstairs downs[down];
    for (i = 0; i < down; i++) {

      //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            downs[i].down_x = (rand() % 79) + 1;
            downs[i].down_y = (rand() % 20) + 1;

            if (dungeon[downs[i].down_y][downs[i].down_x] == '.') {
                dungeon[downs[i].down_y][downs[i].down_x] = '>';
                hardness[downs[i].down_y][downs[i].down_x] = 0;
                isFloor = true;
            }
        }

        isFloor = false;
    }
}
