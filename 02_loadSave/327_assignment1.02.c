#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include "rlg327.c"

void saveDungeon(char*);
void loadDungeon(char*);

/**
 * CS 327: Assignment 1.02: Dungeon Load/Save
 * Project by: Sanjana Amatya, MyTien Kien, Haylee Lawrence
 */

/**
    -- we need to implement hardness this assignment: a room or corridor has hardness of 0, the outer most cells have hardness of 255,
       every other cell has random hardness between 1 - 254 --
    In Main:
        - set up directory
        - finish if statements for --save and --load parameters
    Save function:
        - write to a file (home/username/.rlg327/dungeon)
          - the whole chart in the pdf are what parameters we need for fwrite and fread (that honestly makes a lot more sense)
          - examples of files are in piazza
        - close the file
    Load function:
        - read file (home/username/.rlg327/dungeon)
        - fread (follow the chart on pdf)
        - print dungeon
**/

int main(int argc, char* argv[]) {

    //make the .directory in home and set the directory name for reference
    char *directory = getenv("HOME");
    char *gameDir = ".rlg327";
    char *savefile = "dungeon";
    char *filepath = malloc((strlen(directory) + strlen(gameDir) + strlen(savefile) + 2 + 1) * sizeof(char));

    mkdir(directory, S_IRWXU);
    sprintf(filepath, "%s/%s/%s", directory, gameDir, savefile);

    if (strcmp("--save", argv[1]) == 0)
        saveDungeon(filepath);

    else if (strcmp("--load", argv[1]) == 0)
        loadDungeon(filepath);

    else if ((strcmp("--save", argv[1]) == 0 && strcmp("--load", argv[2]) == 0) || (strcmp("--load", argv[1]) == 0 && strcmp("--save", argv[2]) == 0)) {
        saveDungeon(filepath);
        loadDungeon(filepath);
    }

    else {
        //generates and prints dungeon like normal
        initDungeon();
        createRooms();
        printDungeon();
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
    fseek(f, 0, SEEK_SET);
    char file_type[13];
    file_type[12] = '\0';
    fread(file_type, sizeof(char), 12, f);

    //read file version
    fseek(f, 12, SEEK_SET);
    uint32_t version = 0;
    fread(&version, sizeof(uint32_t), 1, f);
    version = be32toh(version);

    //read the size of the file
    fseek(f, 16, SEEK_SET);
    uint32_t size;
    fread(&size, sizeof(uint32_t), 1, f);
    size = be32toh(size);

    //pc - do we have to convert to big endian here? -- check
    fseek(f, 20, SEEK_SET);
    uint8_t pc_x;
    uint8_t pc_y;
    fread(&pc_x, sizeof(uint8_t), 1, f);
    fread(&pc_y, sizeof(uint8_t), 1, f);

    //hardness values
    fseek(f, 22, SEEK_SET);
    int8_t h[21][80]; //do we even need this

    for(int i = 0; i < 21; i++) {
        for(int j = 0; j < 80; j++) {
            //or is it &hardness[i][j]?? -- check
            fread(&h, 1, 1680, f);
        }
    }

    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (hardness[i][j] > 0 && hardness[i][j] < 255)
                dungeon[i][j] = ' ';

            else if (hardness[i][j] == 0)
                dungeon[i][j] = '#';

            else if (hardness[i][j] == 255) {
                //if (i > 18 || i < 2 || j > 77 || j < 2) //dont think we need this -- check
                dungeon[i][j] = '*';
            }
        }
    }

    fseek(f, 1702, SEEK_SET);
    fread(&numRooms, 1, 2, f);
    numRooms = be32toh(numRooms);

    fseek(f, 1704, SEEK_SET);
    for (int i = 0; i < numRooms; i+=4) {
        fread(&rooms[i], 1, 1, f);
        fread(&rooms[i + 1], 1, 1, f);
        fread(&rooms[i + 2], 1, 1, f);
        fread(&rooms[i + 3], 1, 1, f);
    }

    //setting rooms
    int topX, topY, xSize, ySize;
    for (int i = 0; i < numRooms; i+=4) {

        topX = rooms[i];
        topY = rooms[i + 1];
        xSize = rooms[i + 2];
        ySize = rooms[i + 3];

        for (int j = topX; j < topX + xSize; j++) {
            for (int k = topY; k < topY + ySize; k++){
                if (hardness[j][k] == 0)
                    dungeon[j][k] = '.';
            }
        }
    }

    //stairs -- need to do

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
    fseek(f, 0, SEEK_SET);
    char file_type[13] = "RLG327-S2021";
    fwrite(&file_type, sizeof(char), 12, f);
    //file_type = be32toh(file_type); //this brings up an error for some reason.. -- commenting out for now

    //file version marker set to 0
    fseek(f, 12, SEEK_SET);
    uint32_t version = 0;
    fwrite(&version, sizeof(uint32_t), 1, f); //is it 1 or is it 4??
    version = be32toh(version);

    //size of files
    fseek(f, 16, SEEK_SET);
    uint32_t size;
    fwrite(&size, sizeof(uint32_t), 1, f);
    size = be32toh(size);

    //PC
    fseek(f, 20, SEEK_SET);
    uint8_t pc_x;
    uint8_t pc_y;
    fwrite(&pc_x, sizeof(uint8_t), 1, f);
    fwrite(&pc_y, sizeof(uint8_t), 1, f);

    //hardness values
    fseek(f, 22, SEEK_SET);
    //int8_t h[21][80]; //do we even need this

    for(int i = 0; i < 21; i++) {
        for(int j = 0; j < 80; j++) {
            //or is it &hardness[i][j] or &h?? -- check
            fread(&hardness[i][j], 1, 1680, f);
        }
    }

    //rooms
    //num of rooms
    fseek(f, 1702, SEEK_SET);
    fwrite(&numRooms, 1, 2, f);
    numRooms = be32toh(numRooms);

    //rooms x, y, and dimensions
    fseek(f, 1704, SEEK_SET);
    //maybe? - no endian conversion needed
    for (int i = 0; i < numRooms; i+=4) {
        fwrite(&rooms[i], 1, 1, f);
        fwrite(&rooms[i + 1], 1, 1, f);
        fwrite(&rooms[i + 2], 1, 1, f);
        fwrite(&rooms[i + 3], 1, 1, f);
    }

    // I tried, sorry if it doesnt work :(
    //stairs
    //num of upstairs
    upstairs = malloc(sizeOf((numRooms) * 4))

    fseek(f, 1704 + (numRooms*4), SEEK_SET);
    fwrite(&up, 1, 2, f);
    up = be32toh(up);

    //upstairs' x and y
    fseek(f, 1706 + (numRooms*4), SEEK_SET);
    int upX, upY;
    while(up < 1) {
        for (int i = 0; i < 21; i++) {
            for(int j = 0; j < 80; j++) {
                dungeon[i][j] = "<";
                upstairs[up].upX = j;
                upstairs[up].upY = i;
                up++;
            }
        }
    }

    for (int i = 0; i < up; i++)
    {
        fwrite(&upstairs[i].upX, 1, 1, f);
        fread(&upstairs[i].upY, 1, 1, f);
    }


    //num of downstairs
    downstairs = malloc(sizeOf((numRooms) * 4));

    fseek(f, 1706 + (numRooms*4) + (down * 2), SEEK_SET);
    fwrite(&down, 1, 2, f);
    down = be32toh(down);

    //downstairs' x and y
    fseek(f, 1708 + (numRooms*4) + (up * 2), SEEK_SET);
    int downX, downY;
    while(down < 1) {
        for (int i = 0; i < 21; i++) {
            for(int j = 0; j < 80; j++) {
                dungeon[i][j] = ">";
                downstairs[down].downX = j;
                downstairs[down].downY = i;
                down++;
            }
        }
    }

    for (int i = 0; i < down; i++)
    {
        fwrite(&downstairs[i].downX, 1, 1, f);
        fread(&downstairs[i].downY, 1, 1, f);
    }

    //close file
    fclose(f);
}
