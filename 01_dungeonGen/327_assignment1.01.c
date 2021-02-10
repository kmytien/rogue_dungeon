#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define WORLD_ROW 21
#define WORLD_COL 80

void printDungeon();
void initDungeon();
void createRooms();
void createCorridors(int*, int);
void staircase();
void printCorridors(int, int, int, int);
int randomRange(int, int);
bool legality(int, int, int, int);

char dungeon[WORLD_ROW][WORLD_COL];

/**
 * Assignment 1.01: Dungeon Generation
 * Project by:
 */
int main(int argc, char* argv[]) {

    //initialize dungeon
    initDungeon();

    //generating dungeon
    //calls createCorridors and staircase()
    createRooms();

    //print dungeon
    printDungeon();
}

//prints Dungeon - no border set for ease of functionality
void printDungeon() {
    int row, col;

    for (row = 0; row < WORLD_ROW; row++) {
        for(col = 0; col < WORLD_COL; col++) {

            //if space is null/not filled then it is rock
            if (dungeon[row][col] == NULL)
                printf(" ");

            //else just print the dungeon value
            else
                printf("%c", dungeon[row][col]);
        }

        printf("\n");
    }
}

//dungeon initialization function: make border rock, set hardness of every material in every cell
void initDungeon() {
    // make outside edges rock, all else remains null
    int row, col;

    for (row = 0; row < WORLD_ROW; row++) {
        for (col = 0; col < WORLD_COL; col++) {
            // if a border edge then set as rock
            if (row == 0 || row == WORLD_ROW - 1 || col == 0 || col == WORLD_COL - 1)
                dungeon[row][col] = ' ';
        }
    }
}

//random room generator function
void createRooms() {
    //makes at least 6 rooms, with max up to 10
    //x-direction can be 4 to 12 blocks
    //y-direction can be 3 to 9 blocks

    int maxRooms = randomRange(6, 10);
    int* rooms = (int*) malloc(maxRooms * 4 * sizeof(int));
    int currentRooms = 0;

    //keeps adding room until it gets to randomized max num of rooms
    while(currentRooms < maxRooms) {

        //getting random room sizes
        int rand_vertical = randomRange(3, 9); //row
        int rand_horizontal = randomRange(4, 12); //col

        //getting random placement (-2 and +2 so we don't touch the border)
        int row_start = randomRange(2, 19);
        int col_start = randomRange(2, 79);
        int i, j;

        //legality checks if its legal to place room there
        if (legality(row_start, col_start, rand_vertical, rand_horizontal)) {

            //adds room to dungeon
            for (i = row_start; i < row_start + rand_vertical; i++ ) {
                for (j = col_start; j < col_start + rand_horizontal; j++) {
                    dungeon[i][j] = '.';
                }
            }

            //add to the rooms array
            rooms[4 * currentRooms] = row_start;
            rooms[4 * currentRooms + 1] = col_start;
            rooms[4 * currentRooms + 2] = rand_vertical;
            rooms[4 * currentRooms + 3] = rand_horizontal;
        }

        currentRooms++;
    }

    //create rooms then corridors
    createCorridors(rooms, maxRooms);
}

//checks if placement of room is legal
bool legality(int startRow, int startCol, int endRow, int endCol) {
    int i, j;

    for (i = startRow - 1; i < endRow + 1; i++) {
        for (j = startCol - 1; j < endCol + 1; j++) {
            if (dungeon[i][j] == '.')
                return false;
        }
    }

    return true;
}

//produces random range based on numbers
int randomRange(int lower, int upper) {
    srand(time(NULL));
    int num;

    return num = (rand() % (upper - lower + 1)) + lower;
}

//structure for room
struct room {
    int xstart; //top left row
    int ystart; //top left col
    int rows; //size vertically
    int cols; //size horizontally
};

//corridor maker - connects rooms and has twists to make it funky
//num is the number of rooms made
void createCorridors(int* rooms, int num) {
    //corridors can't extend into rooms

    int i, currRoom = 0;
    struct room firstRoom;
    struct room secondRoom;

    while (currRoom < num) {
        firstRoom.xstart = rooms[4 * currRoom];
        firstRoom.ystart = rooms[4 * currRoom + 1];
        firstRoom.rows = rooms[4 * currRoom + 2]; //dont think this is needed
        firstRoom.cols = rooms[4 * currRoom + 3]; //dont think this is needed

        //int distance = 10000.0;
        //use euclidean distance to its centroid to find closest room
        //set for secondRoom struct

        int x, y, lowest = 80; //should I set lowest to 80
        for (i = currRoom + 1; i < num; i++) {
            x = abs(firstRoom.xstart - rooms[4 * i]);
            y = abs(firstRoom.ystart - rooms[4 * i + 1]);

            if (sqrt(x*x + y*y) < lowest) {
                lowest = sqrt(x * x + y * y);

                //closest room
                secondRoom.xstart = rooms[4 * i];
                secondRoom.ystart = rooms[4 * i + 1];
                secondRoom.rows = rooms[4 * i + 2]; //dont think this is needed
                secondRoom.cols = rooms[4 * i + 3]; //dont think this is needed
            }
        }

        //actually prints it in dungeon
        printCorridors(firstRoom.xstart, firstRoom.ystart, secondRoom.xstart, secondRoom.ystart);
        currRoom++;
    }

    staircase();
    //After placing rooms, move through the room array of n rooms, connecting room 1 with room 2, then room 3 with
    //rooms 1–2, . . . until you’ve connected room n with rooms 1–(n − 1). ex connect room 9 with rooms 1 thru 8
    //carve a path to it by changing rock to corridor
}

//draws corridors
void printCorridors(int fX, int fY, int eX, int eY) {

    while (fX < eX && fY < eY) {
        if (fX < eX) {
            for (int i = fX; i < eX; i++) {
                if (dungeon[fX][fY] == ' ')
                    dungeon[fX][fY] == '#';
            }
        }

        else {
            for (int i = fX; i > eX; i--) {
                if (dungeon[fX][fY] == ' ')
                    dungeon[fX][fY] == '#';
            }
        }

        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[fX][fY] == ' ')
                    dungeon[fX][fY] == '#';
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[fX][fY] == ' ')
                    dungeon[fX][fY] == '#';
            }
        }
    }

}

//up staircase with '<' and down staircase with '>'
void staircase() {
    int stairs, i, row, col;
    bool isFloor = false;

    //randomly generate num between 1 and 3 and then place 1-3 of each staircase in a room
    //up stairs '<'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {

        //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            col = (rand() % 79) + 1;
            row = (rand() % 20) + 1;

            if (dungeon[row][col] == '.') {
                dungeon[row][col] = '<';
                isFloor = true;
            }
        }

        isFloor = false;
    }

    //down stairs '>'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {

        //pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isFloor) {
            col = (rand() % 79) + 1;
            row = (rand() % 20) + 1;

            if (dungeon[row][col] == '.') {
                dungeon[row][col] = '>';
                isFloor = true;
            }
        }

        isFloor = false;
    }
}
