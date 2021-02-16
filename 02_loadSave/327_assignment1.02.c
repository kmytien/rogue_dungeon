#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <endian.h>
#include "rlg327.c"


void file_implementation();
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
    // make the .directory in home and set the directory name for reference
    FILE *file;

    char *directory = getenv("HOME");
//     char *filepath = malloc(strlen(strcat(directory, "./rlg327/dungeon")));
//     strcat(directory, "./rlg327/dungeon");
    char *gameDir = ".
    char *savefile = "dungeon";
    char *filepath = malloc(strlen(directory) + strlen(gameDir) + strlen(savefile) + 2 + 1);
    mkdir(directory, S_IRWXU);

    // sprintf(filepath, "%s/&s/%s", home, gameDir, savefile);
    sprintf(filepath, "%s/&s/%s", home, gameDir, savefile);
    mkdir(directory, S_IRWXU);

    file = fopen(directory, "wb+");

    if(file == NULL) {
        fprintf(stderr, "ERROR, Could not open dungeon file");
        exit(1);
    }

    if (strcmp("--save", argv[1]) == 0)
        saveDungeon(filepath);

    //if user puts --load in command line
    else if (strcmp("--load", argv[1]) == 0)
        loadDungeon(filepath);

    //if user puts --save and --load in command line
    else if ((strcmp("--save", argv[1]) && strcmp("--load", argv[2])) || strcmp("--save", argv[2]) && strcmp("--load", argv[1])) {
        //reads the dungeon from disk, displays it, rewrites it, and exits
        //is it save then load or load then save?
        saveDungeon(filepath);
        loadDungeon(filepath);
    }

    else {
        //generates and prints dungeon like normal
    }
}


void file_implementation(file) {
    //fseek - moves file pointer position to certain location (wherever you put in parenthesis)
    //SEEK_SET - moves file pointer position to the beginning of the file
    //fread - reads data into an array

    //read file type marker
    fseek(file, 0, SEEK_SET);
    char marker[4];
    fread(marker, 1, 4, file);

    // read file version
    fseek(file, 4, SEEK_SET);
    //uint32_t - something to do with guaranteeing 32 bits, can declare pointer types/ files with it??
    uint32_t version;
    uint32_t read_value;
    fread(&read_value,sizeof(uint32_t), 1, file);
    version = be32toh(read_value);

    // read the size of the file
    fseek(file, 4, SEEK_SET);
    uint32_t size;
    uint32_t be_size;
    fread(&be_size, sizeof(uint32_t), 1, file);
    size = be32toh(be_size);

    // write file type marker
    fseek(file, 0, SEEK_SET);
    char marker2[4];
    strcpy(marker2, "RLG327");
    fwrite(marker2, sizeof(char), 4, file);

    // write file version
    fseek(file, 4, SEEK_SET);
    uint32_t version2 = 0;
    uint32_t read_value2 = htobe32(version2);
    fwrite(&read_value2, sizeof(uint32_t), 1, file);

    // write size of file?

    // hardness values? and whatever they have to do with files
}



void saveDungeon(char* filepath) {
    FILE* f = fopen(filepath, "w");

    //using fwrite(&pointer to where elements are written, offset, num bytes, file)

}


//this is where the PC would be loaded
void loadDungeon(char* filepath) {
    FILE* f = fopen(filepath, "r");

    //would use fread

    //would print characters of dungeon based on hardness

    //print dungeon

    //file close

}
