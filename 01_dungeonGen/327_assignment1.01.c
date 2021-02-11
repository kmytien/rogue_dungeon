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
 * Project by: Sanjana Amatya, MyTien Kien, Haylee Lawrence
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

            if (row == 0 || row == WORLD_ROW - 1)
                printf("-");

            else if (col == 0 || col == WORLD_COL - 1)
                printf("|");

            //this is immutable
            else if (dungeon[row][col] == '@')
                printf(" ");

            else if (dungeon[row][col] == '#')
                printf("#");

            else
                printf("%c", dungeon[row][col]);
        }

        printf("\n");
    }
}

//dungeon initialization function: make border rock, set hardness of every material in every cell
void initDungeon() {

    //make outside edges rock, all else remains null
    int row, col;

    for (row = 0; row < WORLD_ROW; row++) {
        for (col = 0; col < WORLD_COL; col++) {

            //if outermost cell, set as @ - idk just a random character
            if (row > WORLD_ROW - 3 || row < 2 || col > WORLD_COL - 3 || col < 2)
                dungeon[row][col] = '@';

            else
                dungeon[row][col] = ' ';
        }
    }
}

//random room generator function
void createRooms() {
    srand(time(NULL));
    //makes at least 6 rooms, with max up to 10
    //x-direction can be 4 to 12 blocks
    //y-direction can be 3 to 9 blocks

    int maxRooms = 6 + (rand() % 5);//randomRange(6, 10);
    int* rooms = (int*) malloc(maxRooms * 4 * sizeof(int));
    int currentRooms = 0;

    //keeps adding room until it gets to randomized max num of rooms
    while(currentRooms < maxRooms) {
        //srand(time(NULL));

        //getting random room sizes
        int rand_vertical = 3 + (rand() % 7);//randomRange(3, 9); //row
        int rand_horizontal = 4 + (rand() % 9);//randomRange(4, 12); //col

        //getting random placement
        int row_start = 2 + (rand() % 17);//randomRange(2, 19);
        int col_start = 2 + (rand() % 77);//randomRange(2, 79);
        int i, j;

        //legality checks if its legal to place room there
        if (legality(row_start, col_start, rand_vertical, rand_horizontal)) {

            //adds room to dungeon
            for (i = row_start; i < row_start + rand_vertical; i++) {
                for (j = col_start; j < col_start + rand_horizontal; j++) {
                    dungeon[i][j] = '.';
                }
            }

            //add to the rooms array
            rooms[4 * currentRooms] = row_start;
            rooms[(4 * currentRooms) + 1] = col_start;
            rooms[(4 * currentRooms) + 2] = rand_vertical;
            rooms[(4 * currentRooms) + 3] = rand_horizontal;

            currentRooms++;

        }
    }

    //For testing
    /**
    for (int k = 0; k < 4*maxRooms; k++) {
        printf("%d ", rooms[k]);
    }
    printf("\n\n");
    **/

    //create rooms then corridors
    createCorridors(rooms, maxRooms);
}

//checks if placement of room is legal
bool legality(int startRow, int startCol, int endRow, int endCol) {
    int i, j;

    for (i = startRow - 1; i < startRow + endRow + 1; i++) {
        for (j = startCol - 1; j < startCol + endCol + 1; j++) {
            if (dungeon[i][j] == '.' || dungeon[i][j] == '@')
                return false;
        }
    }

    return true;
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

    int i, currRoom = 0;
    struct room firstRoom;
    struct room secondRoom;

    for (i = currRoom; currRoom < num; currRoom++) {
        firstRoom.xstart = rooms[4 * currRoom];
        firstRoom.ystart = rooms[(4 * currRoom) + 1];

        //set for secondRoom struct
        if (i == num - 1) {
            secondRoom.xstart = rooms[0];
            secondRoom.ystart = rooms[1];
        }

        else {
            secondRoom.xstart = rooms[(4 * currRoom) + 4];
            secondRoom.ystart = rooms[(4 * currRoom + 1) + 4];
        }

        //actually prints it in dungeon
        printCorridors(firstRoom.xstart, firstRoom.ystart, secondRoom.xstart, secondRoom.ystart);
    }

    staircase();
}

//draws corridors
void printCorridors(int fX, int fY, int eX, int eY) {
    //FOR TESTING
    //printf("%d rowstart\n", fX);
    //printf("%d rowend\n", eX);
    //printf("%d colstart\n", fY);
    //printf("%d colend\n\n", eY);

    if (fX < eX) {
        for (int i = fX; i < eX; i++) {
            if (dungeon[i][fY] == ' ')
                dungeon[i][fY] = '#';
        }

        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ')
                    dungeon[eX][i] = '#';
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ')
                    dungeon[eX][i] = '#';
            }
        }
    }

    else {
        for (int i = fX; i > eX; i--) {
            if (dungeon[i][fY] == ' ')
                dungeon[i][fY] = '#';
        }

        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ')
                    dungeon[eX][i] = '#';
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ')
                    dungeon[eX][i] = '#';
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
