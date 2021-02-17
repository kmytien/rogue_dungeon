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
    //fopen(filepath, "w"); - we dont have to open file yet

    if (strcmp("--save", argv[1]) == 0)
        saveDungeon(filepath);

    else if (strcmp("--load", argv[1]) == 0)
        loadDungeon(filepath);

    else if ((strcmp("--save", argv[1]) && strcmp("--load", argv[2])) || strcmp("--load", argv[1]) && strcmp("--save", argv[2])) {
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
    int row, col, rooms;
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
                if (i > 18 || i < 2 || j > 77 || j < 2)
                    dungeon[i][j] = '@'; // prob have to change a few things e.o - bc @ represents player and here o, representing it as immutable hardness
            }
        }
    }

    //reads rooms
    fseek(f, 1702, SEEK_SET);
    int count = malloc(size - 1704);
    Room rooms = count / 4;
    fread(count, 1, (size - 1704), f);

    int i, j = 0;
    for(i = 0; i < (size - 1704)/4; i++) {
        int x, y, w, h; // w and h are width and height

        //look into struct in 1.01
        rooms[i].xstart = count[j];
        rooms[i].ystart = count[j + 1];
        rooms[i].w = count[j + 2];
        rooms[i].h = count[j + 3];

        /*fread(&x, sizeof(int8_t), 1, f);
        fread(&y, sizeof(int8_t), 1, f);
        fread(&w, sizeof(int8_t), 1, f);
        fread(&h, sizeof(int8_t), 1, f);*/
    }

    // plot rooms in dungeon based on read from files
    int o, p, q;
    for(o = 0; i < (size - 1704)/4; o++) {
      for(p = rooms[o].y; p < rooms[o].y + rooms[o].h; p++) {
        for(q = rooms[o].x; q < rooms[o].x + rooms[o].w; q++) {
          dungeon[p][q] = '.';
        }
      }
    }

    //stairs

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
    file_type = be32oh(file_type);

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
    int8_t h[21][80]; //do we even need this

    for(int i = 0; i < 21; i++) {
        for(int j = 0; j < 80; j++) {
            //or is it &hardness[i][j]?? -- check
            fread(&h, 1, 1680, f);
        }
    }

    //writes rooms
    // GLOBAL? VARIABLE IN 1.01 IS tot_rooms I THINK WE CAN ACCESS IT W/O ISSUE
    uint8_t rooms[tot_rooms][4];
    // for loop to write the corrdinates x and y then the x size (width) and then y length (height)
    fwrite(&rooms, 1, 4*tot_rooms, f);


    //close file
    fclose(f);

}
