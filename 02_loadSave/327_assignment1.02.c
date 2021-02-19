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
