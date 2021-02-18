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
        if (strcmp("--save", argv[1]) == 0)
            saveDungeon(filepath);

        else if (strcmp("--load", argv[1]) == 0)
            loadDungeon(filepath);

        else if ((strcmp("--save", argv[1]) == 0 && strcmp("--load", argv[2]) == 0) ||
                 (strcmp("--load", argv[1]) == 0 && strcmp("--save", argv[2]) == 0)) {
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

    if (hardness[(int) pc_y][(int) pc_x] == 0) {
        dungeon[(int) pc_y][(int) pc_x] = '@';
    }

    //hardness values
    fseek(f, 22, SEEK_SET);
    int8_t h[21][80]; //do we even need this
    fread(&h, 1, 1680, f);

    for(int i = 0; i < 21; i++) {
        for(int j = 0; j < 80; j++) {
            //or is it &h[][] or &hardness[i][j]?? -- check
            hardness[i][j] = h[i][j];
        }
    }

    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (hardness[i][j] > 0 && hardness[i][j] < 255)
                dungeon[i][j] = ' ';

            else if (hardness[i][j] == 0)
                dungeon[i][j] = '#';

            else if (hardness[i][j] == 255) {
                if (i > 18 || i < 2 || j > 77 || j < 2) //dont think we need this -- check
                    dungeon[i][j] = '*';
            }
        }
    }

    fseek(f, 1702, SEEK_SET);
    fread(&numRooms, 1, 2, f);
    numRooms = be32toh(numRooms);

    fseek(f, 1704, SEEK_SET);
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
                if (hardness[j][k] == 0)
                    dungeon[j][k] = '.';
            }
        }
    }

    //stairs
    //num of upstairs
    //upS = malloc(up * sizeof(struct upstairs));
    fseek(f, 1704 + (numRooms*4), SEEK_SET);
    fread(&up, 1, 2, f);
    up = be32toh(up);

    //upstairs' x and y
    fseek(f, 1706 + (numRooms*4), SEEK_SET);
    for (int i = 0; i < up; i++) {
        fread(&upS[i]->up_x, 1, 1, f);
        fread(&upS[i]->up_y, 1, 1, f);
    }

    //setting upstairs
    int upX, upY;
    for (int i = 0; i < up; i++) {
        upX = upS[i]->up_x;
        upY = upS[i]->up_y;

        if (hardness[upY][upX] == 0)
            dungeon[upY][upX] = '<';
    }

    //num of downstairs
    //downS = malloc(down * sizeof(struct downstairs));
    fseek(f, 1706 + (numRooms*4) + (up * 2), SEEK_SET);
    fread(&down, 1, 2, f);
    down = be32toh(down);

    //downstairs' x and y
    fseek(f, 1708 + (numRooms*4) + (up * 2), SEEK_SET);

    for (int i = 0; i < down; i++) {
        fread(&downS[i]->down_x, 1, 1, f);
        fread(&downS[i]->down_y, 1, 1, f);
    }

    //setting upstairs
    int downX, downY;
    for (int i = 0; i < up; i++) {
        downX = downS[i]->down_x;
        downY = downS[i]->down_y;

        if (hardness[downY][downX] == 0)
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
    fseek(f, 0, SEEK_SET);
    char file_type[13] = "RLG327-S2021";
    fwrite(file_type, sizeof(char), 12, f);
    //file_type = be32toh(file_type); //this brings up an error for some reason.. -- commenting out for now

    //file version marker set to 0
    fseek(f, 12, SEEK_SET);
    uint32_t version = 0;
    version = be32toh(version);
    fwrite(&version, sizeof(uint32_t), 1, f); //is it 1 or is it 4??

    //size of files
    fseek(f, 16, SEEK_SET);
    uint32_t size;
    size = be32toh(size);
    fwrite(&size, sizeof(uint32_t), 1, f);

    //PC
    fseek(f, 20, SEEK_SET);
    uint8_t pc_x;
    uint8_t pc_y;
    fwrite(&pc_x, sizeof(uint8_t), 1, f);
    fwrite(&pc_y, sizeof(uint8_t), 1, f);

    //hardness values
    fseek(f, 22, SEEK_SET);
    int8_t h[21][80]; //do we even need this
    fwrite(&h, 1, 1680, f);
    
    for(int i = 0; i < 21; i++) {
        for(int j = 0; j < 80; j++) {
            //or is it &hardness[i][j] or &h?? -- check
            h[i][j] = hardness[i][j];
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
        fwrite(&roomsDimensions[i], 1, 1, f);
        fwrite(&roomsDimensions[i + 1], 1, 1, f);
        fwrite(&roomsDimensions[i + 2], 1, 1, f);
        fwrite(&roomsDimensions[i + 3], 1, 1, f);
    }

    //stairs
    //num of upstairs
    //upS = malloc(up * sizeof(struct upstairs));
    fseek(f, 1704 + (numRooms*4), SEEK_SET);
    fwrite(&up, 1, 2, f);
    up = be32toh(up);

    //upstairs' x and y
    fseek(f, 1706 + (numRooms*4), SEEK_SET);
    for (int i = 0; i < up; i++) {
        fwrite(&upS[i]->up_x, 1, 1, f);
        fwrite(&upS[i]->up_y, 1, 1, f);
    }

    //num of downstairs
    //downS = malloc(down * sizeof(struct downstairs));
    fseek(f, 1706 + (numRooms*4) + (up * 2), SEEK_SET);
    fwrite(&down, 1, 2, f);
    down = be32toh(down);

    //downstairs' x and y
    fseek(f, 1708 + (numRooms*4) + (up * 2), SEEK_SET);

    for (int i = 0; i < down; i++) {
        fwrite(&downS[i]->down_x, 1, 1, f);
        fwrite(&downS[i]->down_y, 1, 1, f);
    }

    //close file
    fclose(f);
}
