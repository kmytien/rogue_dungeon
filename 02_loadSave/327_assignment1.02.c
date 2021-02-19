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


    if (argc > 1) {
	if (argc == 3) {
		if ((strcmp("--save", argv[1]) == 0 && strcmp("--load", argv[2]) == 0) || (strcmp("--load", argv[1]) == 0 && strcmp("--save", argv[2]) == 0)) {
		    loadDungeon(filepath);
		    saveDungeon(filepath);
		}
        }
	else if (argc == 2) {
		if (strcmp("--save", argv[1]) == 0) {
		    initDungeon();
		    createRooms();
		    saveDungeon(filepath);
		}

		else if (strcmp("--load", argv[1]) == 0) {
		    loadDungeon(filepath);
		}
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
    fread(&file_type, 1, 12, f);

    //read file version
    uint32_t version;
    fread(&version, 4, 1, f);

    //read the size of the file
    uint32_t size;
    fread(&size, 4, 1, f);
    size = be32toh(size);

    //pc
    fread(&pc.row, sizeof(uint8_t), 1, f);
    fread(&pc.col, sizeof(uint8_t), 1, f);

    if (d.hardness[pc.row][pc.col] == 0) {
        d.dungeon[pc.row][pc.col] = '@';
    }

    //hardness values
    fread(&d.hardness, 1, 1680, f);

    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            if (d.hardness[i][j] > 0 && d.hardness[i][j] < 255)
                d.dungeon[i][j] = ' ';

            else if (d.hardness[i][j] == 0)
                d.dungeon[i][j] = '#';

            else if (d.hardness[i][j] == 255) {
                d.dungeon[i][j] = '*';
            }
        }
    }

    uint16_t num;
    fread(&num, 1, 2, f);
    d.numRooms = be16toh(num);
    
    rooms = malloc(20 * sizeof(struct room));
    uint8_t roomInfo;
    for (int i = 0; i < d.numRooms; i++) {
    
        fread(&roomInfo, 1, 1, f);
        rooms[i].xstart = roomInfo;
        fread(&roomInfo, 1, 1, f);
        rooms[i].ystart = roomInfo;
        fread(&roomInfo, 1, 1, f);
        rooms[i].xsize = roomInfo;
        fread(&roomInfo, 1, 1, f);
        rooms[i].ysize = roomInfo;
    }

    //setting rooms
    int topX, topY, xSize, ySize;
    for (int i = 0; i < d.numRooms; i++) {
        
        topX = rooms[i].xstart;
        topY = rooms[i].ystart;
        xSize = rooms[i].xsize;
        ySize = rooms[i].ysize;

        for (int j = topX; j < topX + xSize; j++) {
            for (int k = topY; k < topY + ySize; k++){
                if (d.hardness[j][k] == 0)
                    d.dungeon[j][k] = '.';
            }
        }
    }

    //stairs
    //num of upstairs
    fread(&up, 1, 2, f);
    up = be32toh(up);

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
            d.dungeon[upY][upX] = '<';
    }

    //num of downstairs
    fread(&down, 1, 2, f);
    down = be32toh(down);

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
            d.dungeon[downY][downX] = '>';
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
    fwrite(&version, 4, 1, f);

    //size of files
    uint32_t size = 1702 + (4*d.numRooms);
    size = htobe32(size);
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
    uint16_t num = htobe16(d.numRooms);
    fwrite(&num, 1, 2, f);

    //rooms x, y, and dimensions
    uint8_t roomInfo;
    for (int i = 0; i < d.numRooms; i++) {
        roomInfo = rooms[i].xstart;
        fwrite(&roomInfo, 1, 1, f);
        roomInfo = rooms[i].ystart;
        fwrite(&roomInfo, 1, 1, f);
        roomInfo = rooms[i].xsize;
        fwrite(&roomInfo, 1, 1, f);
        roomInfo = rooms[i].ysize;
        fwrite(&roomInfo, 1, 1, f);
    }

    //stairs
    //num of upstairs
    fwrite(&up, 1, 2, f);
    up = be32toh(up);

    //upstairs' x and y
    for (int i = 0; i < up; i++) {
        fwrite(&ups[i].up_x, 1, 1, f);
        fwrite(&ups[i].up_y, 1, 1, f);
    }

    //num of downstairs
    fwrite(&down, 1, 2, f);
    down = be32toh(down);

    //downstairs' x and y
    for (int i = 0; i < down; i++) {
        fwrite(&downs[i].down_x, 1, 1, f);
        fwrite(&downs[i].down_y, 1, 1, f);
    }


    //close file
    fclose(f);
}
