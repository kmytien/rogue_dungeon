#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <stdbool.h>
//#include <unistd.h>
#include <time.h>
#include <math.h>

#define WORLD_ROW 21
#define WORLD_COL 80

void printDungeon();
void initDungeon();
void createRooms();
void createCorridors(int*, int);
void staircase();
int randomRange(int, int);
bool legality(int, int, int, int);

char dungeon[WORLD_ROW][WORLD_COL];

/**
RULES FOR THE DUNGEON MAP
• Dungeon measures 80 units x direction and 21 units y direction
  - he said something about 24 units y direction, accounting for text and everything but idk if we need to include it here o_o
• at least 6 rooms per dungeon
• Each room measures at least 4 units in the x direction and at least 3 units in the y direction.
• Rooms need not be rectangular, but neither may they contact one another. There must be at least 1
cell of non-room between any two different rooms.
• The outermost cells of the dungeon are immutable, thus they must remain rock and cannot be part of
any room or corridor. ***(THE BORDER????) -HL
• At least one down and one up staircase (> and < respectively). Placed in a location where floor would be and must have at least one side open to access it
• Room (.), corridor cells (#), rock ( ), up staircases (<), and down staircases (>)
• The dungeon should be fully connected, meaning that from any position on the floor, your adventurer should be able to walk to any other position
on the floor without passing through rock.
• Corridors should not extend into rooms, e.g., no hashes should be rendered inside rooms.
**/

// from the lecture - he said we might need to use pointers and malloc - WHAT IS MALLOC

// main function to execute the dungeon creator
int main(int argc, char* argv[]) {

    // initialize dungeon
    initDungeon();

    // populate dungeon - generating dungeon
    createRooms(); // calls createCorridors
    staircase();

    // print dungeon
    printDungeon();

}

// print function NO BORDER SET FOR EASE OF FUNCTIONALITY
void printDungeon() {
    int row, col;

    // we can just go thru the whole grid and printf("%c", dungeon[row][col]) since its a 2D array of chars
    // 80 units horizontally length and 21 vertically width if we leave out 3 lines for text
    for (row = 0; row < WORLD_ROW; row++) {
        for(col = 0; col < WORLD_COL; col++) {

            // if space is null/ not filled then it is rock
            if (dungeon[row][col] == NULL)
                printf(" ");

            // else just print the dungeon value
            else
                printf("%c", dungeon[row][col]);
        }

        printf("\n");
    }
}



// dungeon initialization function: make border rock, set hardness of every material in every cell (IF TIME)
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

// random room generator function
// rooms are represented by periods '.'
void createRooms() {
    // rooms can't be in contact with each other/walls of dungeon can't be created by wall of rooms
    // Min/Max Rooms: AT LEAST 6 -> AT MOST 10
    // Min Room Width (left right) = 4, Max Room Width = 12 row
    // Min Room length (up down) = 3, Max Room Length = 9 col

    //malloc is used when we don't know how much memory we should allocate
    //probably best used in this method and also methods that has a randomized number
    int maxRooms = randomRange(6, 10);

    //sheaffer mentioned creating array for rooms so im assuming its this
    int* rooms = (int*) malloc(maxRooms * 4 * sizeof(int));
    int currentRooms = 0;

    //keeps adding room until it gets to randomized max num of rooms
    do {

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

            // add to the rooms array
            //i checked in clion and it didn't create a 2d array but i think this works as well -mk
            rooms[4 * currentRooms] = row_start;
            rooms[4 * currentRooms + 1] = col_start;
            rooms[4 * currentRooms + 2] = rand_vertical;
            rooms[4 * currentRooms + 3] = rand_horizontal;
        }

        currentRooms++;
    } while (currentRooms < maxRooms);

    //create rooms then corridors
    createCorridors(rooms, maxRooms);
}

//checks if placement of room is legal
//height up and down (col), length left and right (row)
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
    int xstart;
    int ystart;
    int rows;
    int cols;
};

// corridor maker - connects rooms and has twists to make it FuNkY
// corridors are represented by '#'
void createCorridors(int* rooms, int num) {
    // corridors can't extend into rooms

    int i;
    struct room closest;
    
    closest.xstart = *rooms[num];
    closest.ystart = *rooms[num];
    closest.rows = *rooms[];
    closest.cols = *rooms[];
    int distance = 10000.0; // 10000 is just a random number to make sure they're close enough

    // finds the closest room that is already connected
    for(i = 0; i < num; i++){
        int x = abs(*rooms[num].x - *rooms[i].x);
        int y = abs(*rooms[num].x - *rooms[i].y);

        // Find the closest room in the already connected set using Euclidean distance (pythagorean theorem) to its centroid (row/2, col/2)
        // distance between two rooms and sets a new closest point when there's a small distance
        if(sqrt(x * x + y * y) < distance) {
            distance = sqrt(x * x + y * y);
            closest.x = *rooms[i].x;
            closest.y = *rooms[i].y;
        }

    }
    // After placing rooms, move through the room array of n rooms, connecting room 1 with room 2, then room 3 with
    // rooms 1–2, . . . until you’ve connected room n with rooms 1–(n − 1). ex connect room 9 with rooms 1 thru 8
    



    // carve a path to it by changing rock to corridor

    // If you get that working, then add some random changes of direction in there to make it look a little more exciting.
}


// method for staircase
// up staircase with '<' and down staircase with '>'
void staircase() {
    int stairs, i, row, col;
    bool isFloor = false;
    // randomly generate num between 1 and 3 and then place 1-3 of each staircase in a room
    // up stairs '<'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {
        // pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
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

    // down stairs '>'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {
        // pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
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
