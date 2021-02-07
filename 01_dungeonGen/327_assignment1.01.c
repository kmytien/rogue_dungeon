#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>



/** RULES FOR THE DUNGEON MAP
• Dungeon measures 80 units x direction and 21 units y direction
• at least 6 rooms per dungeon
• Each room measures at least 4 units in the x direction and at least 3 units in the y direction.
• Rooms need not be rectangular, but neither may they contact one another. There must be at least 1
cell of non-room between any two different rooms.
• The outermost cells of the dungeon are immutable, thus they must remain rock and cannot be part of
any room or corridor. ***(THE BORDER????) -HL
• At least one down and one up stairfase (> and < respectively). Placed in a location where floor would be and must have at least one side open to access it
• Room (.), corridor cells (#), rock ( ), up staircases (<), and down staircases (>)
• The dungeon should be fully connected, meaning that from any position on the floor, your adventurer should be able to walk to any other position 
on the floor without passing through rock.
• Corridors should not extend into rooms, e.g., no hashes should be rendered inside rooms.
**/

// main function to execute the dungeon creator
int main(int argc, char* argv[]) {
}

// print function

// random generator function
// BRAINSTORMING??
// WE COULD MAKE A RANDOM NUM GENERATOR THAT GOES TO RANDOM CELLS AND CAN CHOOSE TO DO A CERTAIN TYPE OF CELL IF THERE ARE SIMILAR CELLS NEARBY, ELSE IT MAKES A CORRIDOR OR 
// SOMETHING?????????? -HL



