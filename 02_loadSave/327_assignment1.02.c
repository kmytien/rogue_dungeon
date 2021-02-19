#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include "rlg327.c"
#include <stdbool.h>
#include <time.h>

#define WORLD_ROW 21
#define WORLD_COL 80

void saveDungeon(char*);
void loadDungeon(char*);
void printDungeon();
void initDungeon();
void createRooms();
void staircase();
void spawnPC();
void createCorridors(int*,int);
void setCorridors(int, int, int, int);
bool legality(int, int, int, int);

/**
 * CS 327: Assignment 1.02: Dungeon Load/Save
 * Project by: Sanjana Amatya, MyTien Kien, Haylee Lawrence
 */

char dungeon[WORLD_ROW][WORLD_COL];
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
struct hardness {
    uint8_t hardness[WORLD_ROW][WORLD_COL];
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
                d.hardness[row][col] = 255;
            }

            else {
                dungeon[row][col] = ' ';
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
                    d.hardness[i][j] = 0;
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
                d.hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }
    }


    else {
        for (int i = fX; i > eX; i--) {
            if (dungeon[i][fY] == ' ') {
                dungeon[i][fY] = '#';
                d.hardness[i][fY] = 0;
            }
        }

        //looking at columns
        if (fY < eY) {
            for (int i = fY; i < eY; i++) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
                    d.hardness[eX][i] = 0;
                }
            }
        }

        else {
            for (int i = fY; i > eY; i--) {
                if (dungeon[eX][i] == ' ') {
                    dungeon[eX][i] = '#';
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
            if (dungeon[ups[i].up_y][ups[i].up_x] == '.') {
                dungeon[ups[i].up_y][ups[i].up_x] = '<';
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

            if (dungeon[downs[i].down_y][downs[i].down_x] == '.') {
                dungeon[downs[i].down_y][downs[i].down_x] = '>';
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

        if (dungeon[y][x] == '.') {
            pc.col = x;
            pc.row = y;
            spawn = true;
        }
    }
}

int main(int argc, char* argv[]) {

    //make the .directory in home and set the directory name for reference
    char *directory = getenv("HOME");
    char *gameDir = ".rlg327";
    char *savefile = "dungeon";
    char *filepath = malloc((strlen(directory) + strlen(gameDir) + strlen(savefile) + 2 + 1) * sizeof(char));

    mkdir(directory, S_IRWXU);
    sprintf(filepath, "%s/%s/%s", directory, gameDir, savefile);
    //printf("%s", filepath);


    if (argc > 1) {

        if (strcmp("--save", argv[1]) == 0) {
            initDungeon();
            createRooms();
            printDungeon();
            saveDungeon(filepath);
        }

        else if (strcmp("--load", argv[1]) == 0) {
            loadDungeon(filepath);
        }

        else if ((strcmp("--save", argv[1]) == 0 && strcmp("--load", argv[2]) == 0) || (strcmp("--load", argv[1]) == 0 && strcmp("--save", argv[2]) == 0)) {
            saveDungeon(filepath);
            loadDungeon(filepath);
        }

        //generates and prints dungeon like normal
        return 0;
    }

    else {
        //generates and prints dungeon like normal
        initDungeon();
        createRooms();
        printDungeon();
        return 0;
    }
}


//load-- read a dungeon from the file and print it
void loadDungeon(char* filepath) {
    //int row, col;
    FILE* f = fopen(filepath, "r");

    if (f == NULL) {
        fprintf(stderr, "ERROR, Could not open dungeon file");
        exit(1);
    }

    //read semantic file type marker
    char file_type[13];
    fread(&file_type, 4, 1, f);

    //read file version
    uint32_t version;
    fread(&version, 4, 1, f);
    //version = be32toh(version);

    //read the size of the file
    uint32_t size;
    fread(&size, 4, 1, f);
    size = be32toh(size);

    //pc - do we have to convert to big endian here? -- check
    fread(&pc.row, sizeof(uint8_t), 1, f);
    fread(&pc.col, sizeof(uint8_t), 1, f);

    if (d.hardness[pc.row][pc.col] == 0) {
        dungeon[pc.row][pc.col] = '@';
    }

    //hardness values
    fread(&d.hardness, 1, 1680, f);

    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (d.hardness[i][j] > 0 && d.hardness[i][j] < 255)
                dungeon[i][j] = ' ';

            else if (d.hardness[i][j] == 0)
                dungeon[i][j] = '#';

            else if (d.hardness[i][j] == 255) {
                dungeon[i][j] = '*';
            }
        }
    }


    fread(&numRooms, 1, 2, f);
    numRooms = be32toh(numRooms);

    for (int i = 0; i < numRooms; i+=4) {
        fread(&roomsDimensions[i], 1, 1, f);
        fread(&roomsDimensions[i + 1], 1, 1, f);
        fread(&roomsDimensions[i + 2], 1, 1, f);
        fread(&roomsDimensions[i + 3], 1, 1, f);
    }

    //setting rooms
    int topX, topY, xSize, ySize;
    for (int i = 0; i < numRooms; i+=4) {

        topX = roomsDimensions[i];
        topY = roomsDimensions[i + 1];
        xSize = roomsDimensions[i + 2];
        ySize = roomsDimensions[i + 3];

        for (int j = topX; j < topX + xSize; j++) {
            for (int k = topY; k < topY + ySize; k++){
                if (d.hardness[j][k] == 0)
                    dungeon[j][k] = '.';
            }
        }
    }

    //stairs
    //num of upstairs
    //upS = malloc(up * sizeof(struct upstairs));
    fread(&up, 1, 2, f);
    up = be32toh(up);
    ups = malloc(sizeof(struct upstairs) * up);

    //upstairs' x and y
    for (int i = 0; i < up; i++) {
        fread(&ups[i].up_x, 1, 1, f);
        fread(&ups[i].up_y, 1, 1, f);
    }

    //setting upstairs
    int upX, upY;
    for (int i = 0; i < up; i++) {
        upX = ups[i].up_x;
        upY = ups[i].up_y;

        if (d.hardness[upY][upX] == 0)
            dungeon[upY][upX] = '<';
    }

    //num of downstairs
    //downS = malloc(down * sizeof(struct downstairs));
    fread(&down, 1, 2, f);
    down = be32toh(down);
    downs = malloc(sizeof(struct downstairs) * down);

    //downstairs' x and y
    for (int i = 0; i < down; i++) {
        fread(&downs[i].down_x, 1, 1, f);
        fread(&downs[i].down_y, 1, 1, f);
    }

    //setting upstairs
    int downX, downY;
    for (int i = 0; i < up; i++) {
        downX = downs[i].down_x;
        downY = downs[i].down_y;

        if (d.hardness[downY][downX] == 0)
            dungeon[downY][downX] = '>';
    }
	

    printDungeon();
    fclose(f);
}

//save dungeon to a file in the directory (write)
void saveDungeon(char* filepath) {

    FILE* f = fopen(filepath, "w");  //filepathpath      pathpath    pathpath      filepathpath

    if (f == NULL) {
        fprintf(stderr, "ERROR, Could not open dungeon file");
        exit(1);
    }

    //file_type
    char file_type[] = "RLG327-S2021";
    fwrite(file_type, 1, 12, f);

    //file version marker set to 0
    uint32_t version = 0;
    // version = be32toh(version);
    fwrite(&version, 4, 1, f); //is it 1 or is it 4??

    //size of files
    uint32_t size = 1702 + (4*numRooms);
    size = be32toh(size);
    fwrite(&size, 4, 1, f);

    //PC
    uint8_t pc_x;
    uint8_t pc_y;
    fwrite(&pc_x, 1, sizeof(uint8_t), f);
    fwrite(&pc_y, 1, sizeof(uint8_t), f);

    //hardness values
    fwrite(&d.hardness, 1, 1680, f);


    //rooms
    //num of rooms
    fwrite(&numRooms, 1, 2, f);
    numRooms = be32toh(numRooms);

    //rooms x, y, and dimensions
    //maybe? - no endian conversion needed
    for (int i = 0; i < numRooms; i+=4) {
        fwrite(&roomsDimensions[i], 1, 1, f);
        fwrite(&roomsDimensions[i + 1], 1, 1, f);
        fwrite(&roomsDimensions[i + 2], 1, 1, f);
        fwrite(&roomsDimensions[i + 3], 1, 1, f);
    }

    //stairs
    //num of upstairs
    //upS = malloc(up * sizeof(struct upstairs));
    fwrite(&up, 1, 2, f);
    up = be32toh(up);

    //upstairs' x and y
    for (int i = 0; i < up; i++) {
        fwrite(&ups[i].up_x, 1, 1, f);
        fwrite(&ups[i].up_y, 1, 1, f);
    }

    //num of downstairs
    //downS = malloc(down * sizeof(struct downstairs));
    fwrite(&down, 1, 2, f);
    down = be32toh(down);

    //downstairs' x and y
    for (int i = 0; i < down; i++) {
        fwrite(&downs[i].down_x, 1, 1, f);
        fwrite(&downs[i].down_y, 1, 1, f);
    }

    //printDungeon();
    //close file
    fclose(f);
}
