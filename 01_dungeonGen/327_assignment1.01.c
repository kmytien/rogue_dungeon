#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define WORLD_LENGTH 21
#define WORLD_WIDTH 80

void printDungeon();
void initDungeon();
void createRooms();
void createCorridors();
void staircase();

char dungeon[WORLD_WIDTH][WORLD_LENGTH];

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
    createRooms();
    createCorridors();
    staircase();

    // print dungeon
    printDungeon();

}

// print function NO BORDER SET FOR EASE OF FUNCTIONALITY
void printDungeon() {
    int row, col;

    // we can just go thru the whole grid and printf("%c", dungeon[row][col]) since its a 2D array of chars
    // 80 units horizontally length and 21 vertically width if we leave out 3 lines for text
    for (row = 0; row < WORLD_LENGTH; row++) {
        for(col = 0; col < WORLD_WIDTH; col++) {
            // if space is null or R then its a ROCK
            if (dungeon[row][col] == "R" || dungeon[row][col] == NULL)
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

    for (row = 0; row = WORLD_LENGTH; row++) {
        for (col = 0; col = WORLD_WIDTH; col++) {
            // if a border edge then set as rock
            if (row == 0 || row == 79 || col == 0 || col == 20)
                dungeon[row][col] = "R";
        }
    }
}

// random room generator function
// rooms are represented by periods '.'
void createRooms() {
    // rooms can't be in contact with each other/walls of dungeon can't be created by wall of rooms
    // Min/Max Rooms: AT LEAST 6 -> AT MOST 10
    // Min Room Width = 4, Max Room Width = 15
    // Min Room length = 3, Max Room Length = 9
    srand(time(NULL));

    //needs at least 6 - 10 rooms with width (4 to 15) and length (3 to 9)
    //6 rooms + (rand() % 5) - (which is 0, 1, 2, 3, 4)
    int maxRooms = 6 + (rand() % 5);
    int currentRooms = 0;

    //should we use malloc here??????? what is malloc :')))))

    //keeps adding room until it gets to randomized max num of rooms
    if (currentRooms < maxRooms) {
        //createRoom function then currentRooms++?
    }

    //create rooms then corridors
    createCorridors();
}

// corridor maker - connects rooms and has twists to make FuNkY
// corridors are represented by '#'
// MK today i learned corridors are hallways
void createCorridors() {
    // corridors can't extend into rooms
}


// method for staircase
// up staircase with '<' and down staircase with '>'
void staircase() {
    int stairs, i, row, col;
    bool isPeriod = false;
    // randomly generate num between 1 and 3 and then place 1-3 of each staircase in a room
    // up stairs '<'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {
        // pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isPeriod) {
            col = (rand() % 79) + 1;
            row = (rand() % 20) + 1;

            if (dungeon[row][col] == '.') {
                dungeon[row][col] = '<';
                isPeriod = true;
            }
        }

        isPeriod = false;
    }

    // down stairs '>'
    stairs = (rand() % 4) + 1;
    for (i = 0; i < stairs; i++) {
        // pick 2 random numbers 1-78, 1-19, if that dungeon cell is a . then set as a stair and exit while loop
        while (!isPeriod) {
            col = (rand() % 79) + 1;
            row = (rand() % 20) + 1;

            if (dungeon[row][col] == '.') {
                dungeon[row][col] = '>';
                isPeriod = true;
            }
        }

        isPeriod = false;
    }
}

