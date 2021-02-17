#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include "rlg327.c"


void saveDungeon(char*);
void loadDungeon(char*);

dungeon_t *dungeon;

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
    // make the .directory in home and set the directory name for reference

    char *directory = getenv("HOME");
    char *gameDir = ".rlg327";
    char *savefile = "dungeon";
    char *filepath = malloc((strlen(directory) + strlen(gameDir) + strlen(savefile) + 2 + 1) * sizeof(char));

    mkdir(directory, S_IRWXU);
    sprintf(filepath, "%s/%s/%s", directory, gameDir, savefile);
    fopen(filepath, "w");

    if(filepath == NULL) {
        fprintf(stderr, "ERROR, Could not open dungeon file");
        exit(1);
    }

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
        gen_dungeon(&*dungeon);
    }
}

//fseek - moves file pointer position to certain location (wherever you put in parenthesis)
//SEEK_SET - moves file pointer position to the beginning of the file
//fread - reads data into an array

//load-- read a dungeon from the file and print it
void loadDungeon(char* filepath) {
    int row, col, rooms;
    FILE* f = fopen(filepath, "r");
    //read semantic file type marker
    fseek(f, 0, SEEK_SET);
    char file_type[13];
    file_type[12] = '\0';
    fread(file_type, 1, 12, f);

    //read file version
    fseek(f, 4, SEEK_SET); //(its like a fancy 32 bit integer)
    uint32_t version;
    uint32_t read_value;
    fread(&read_value, sizeof(uint32_t), 1, f);
    version = be32toh(read_value);

    //read the size of the file
    fseek(f, 4, SEEK_SET);
    int size;
    int be_size;
    fread(&be_size, sizeof(uint32_t), 1, f);
    size = be32toh(be_size);

    //would print characters of dungeon based on hardness
    for (row = 0; row < 21; row++) {
      for (col = 0; col < 80; col++) {
        //if hardness is 255 outermost cells
        if (dungeon.hardness[row][col] -> 255) {
          dungeon[row][col] = ter_wall_immutable;
        }
        //if hardness 0 - corridors/rooms
        else if (dungeon.hardness == 0) {
          dungeon[row][col] = ter_floor; // HOW DO I FIND THE HARDNESS OF STAIRS
        }
        else if(dungeon.hardness > 1 && dungeon.hardness < 255) {
          dungeon[row][col] = ter_wall;
        }
      }
    }
    // add rooms
    // HOW DO I ADD STAIRCASES
    for (rooms = 0; rooms < dungeon.num_rooms; rooms++) {
      for (row = room.position[dim_y]; row < room.size[dim_y]; row++) {
        for (col = room.position[dim_x]; col < room.size[dim_x]; col++) {
          dungeon[row][col] = ter_floor_room;
        }
      }
    }
    //print dungeon
    render_dungeon(dungeon);

    // hardness values? and whatever they have to do with files
    int i, j;
    for(i = 0; i < 21; i++) {
      for(j = 0; j < 80; j++) {
        int8_t h;
        fread(&h, sizeof(int8_t), 1, filepath);
      }
    }




    fclose(f);
}

//save dungeon to a file in the directory
void saveDungeon(char* filepath) {


    FILE* f = fopen(filpath, "w");  //filepathpath      pathpath    pathpath      filepathpath

    //semantic file_type marker
    char semantic[13] = "RLG327-S2021";
    fwrite(&semantic, 1, 12, f);

    //file version marker set to 0
    fseek(f, 0, SEEK_SET);
    uint32_t marker = 0;
    marker = be32toh(marker);
    fwrite(&marker, 1, 4, f);

    //size of files
    uint32_t size;
    size = be32toh(size);
    fwrite(&size, 1, 4, f);

    //PC
    uint8_t PCx;
    uint8_t PCy;
    fwrite(&PCx, 1, 1, f);
    fwrite(&PCy, 1, 1, f);

    //hardness - do we have to do something more here?
    uint8_t hardness[21][80];
    for (int i = 0; i < 21; i++) {
      for (int j = 0; j < 80; j++) {
          fwrite(&hardness, 1, 1680, f);
      }
    }

    //rooms
    uint16_t rooms;

    //close file
    fclose(f);

}

