#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//ive seen people include the header file instead of c file e.g. #include "rlg327.h" -> with that file including all the function names
//but im not sure if this is right either, if it doesn't work using this, we can change to the header file
#include "rlg327.c"

void file_implemetation();
void saveDungeon();
void load();

// i forget how to do strings ahhhhh
#define String filename;

/**
 * CS 327: Assignment 1.02: Dungeon Load/Save
 * Project by: Sanjana Amatya, MyTien Kien, Haylee Lawrence
 */

 /** WHAT TO DO
    1. make a save function (saves game to disk before terminating)
    2. make a load function (loads dungeon from disk instead of making a new one)

    We’ll add two switches, --save and --load. The save switch will cause the game to save the dungeon to disk before
    terminating. The load switch will load the dungeon from disk, rather than generate a new one, then display
    it and exit.
**/

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

    char *directory;
    strcat(directory, "./rlg327"); // might not work since mkdir takes a char pointer and idk this might make a string???? we'll see
    mkdir(directory, S_IRWXU); // not totally sure what S_IRWXU is, but its supposed to imply that the owner will have read/write/execute permissions on the directory
    char *directoryName = strcat(getenv("HOME"), "/", directory, "/dungeon"); //i think strcat can only concatenate 2 things at a time

    //need to look at arguments, have to do diff things with --save, and --load or none
    //if user puts --save in command line


    // fopen creates a new file for dungeon
    // not sure what goes in the parenthesis but it's whatever file we are trying to open
    file = fopen(dungeon, "wb+"); // i think dungeon is what we are trying to open.... idk if its supposed to be in parenthesis - i looked up examples and i don't think so

    if(file == NULL) {
      // apparently printf isn't a think in files, fprintf prints a formatted output to a file
      // stderr is just a standard error
      fprintf(stderr, "ERROR, Could not open dungeon file");
      exit(1);
    }

    if (strcmp("--save", argv[1]) == 0) {
      // saves file to disk (saves file to)
      saveDungeon();
    }

    //if user puts --load in command line
    else if (strcmp("--load", argv[1]) == 0) {
      // load the dungeon from disk, rather than generate a new one then display it and exit.
      load();
    }

    //if user puts --save and --load in command line
    else if ((strcmp("--save", argv[1]) && strcmp("--load", argv[2])) || strcmp("--save", argv[2]) && strcmp("--load", argv[1])) {
      //reads the dungeon from disk, displays it, rewrites it, and exits
      load(); // calls print
      saveDungeon();
    }
    //if user puts nothing/anything else in command line it will automatically call an error
}

// reads from a dungeon file in the directory
void file_implemetation() {



  // fseek - moves file pointer position to certain location (wherever you put in parenthesis)
  // SEEK_SET - moves file pointer position to the beginning of the file
  // fread - reads data into an array
  // Markers

  // need to look at file type markers and how to make markers?
  fseek(file, 0, SEEK_SET);
  char marker[8];
  fread(marker, 1, 8 file);


  // file version markers
  fseek(file, 6, SEEK_SET);
  //uint32_t - something to do with guarenteeing 32 bits, can declare pointer types/ files with it??
  uint32_t file_version;


 // read the size of the file



 // hardness values? and whatever they have to do with files




}

void saveDungeon() {

}

void load() {

}
