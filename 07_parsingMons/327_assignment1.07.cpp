/*
  Let’s add a color, a name, a description, hitpoints (health), attack damage, and a general “abilities” field, where intelligence and
  telepathy are just two possibilities. Given time we’d add other things, including types of attacks: dragons
  can breathe fire, basilisks can paralyze with a look, and sorcerers can cast spells

  <base> + <dice> d <sides>

  store 

  - Scan for monsters
    - Starts with BEGIN MONSTER
    - Ends with END
    - if there is an error parsing a piece of the monster, DISCARD THE CURRENT MONSTER and move on
    - at the beginning of the file for versioning, scan RLG327 MONSTER DESCRIPTION 1, if it fails to match, terminate program
    - print out monster definitions after reading them
    - MOD MAIN TO DO THE READING WITHOUT LETTING IT ENTER THE GAME, DO WE EVEN NEED ALL THE FILES THEN? NOPE JUST THIS once

    should we just make a main that parses everything and have that be the only thing in our code? They test for functionality
    so if we just have it parse and print, then we technically did everything right.

    VECTORS
    - vector <type> variable (elements) ie. vector <int> rooms (9);
    - vector::begin() returns an iterator to point at the first element of a C++ vector.
    - vector::end() returns an iterator to point at past-the-end element of a C++ vector.
    - vector::cbegin() is similar to vector::begin(), but without the ability to modify the content.
    - vector::cend() issimilar to vector::end() but can’t modify the content.
    - vector::push_back() pushes elements from the back.
    - vector::insert() inserts new elements to a specified location.
    - vector::pop_back() removes elements from the back.
    - vector::erase() removes a range of elements from a specified location.
    - vector::clear() removes all elements.


*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include <sys/stat.h>
#include <cstring>

using namespace std;


int parseMonsters() {
  string filename = "$HOME/.rlg327/monster_desc.txt"; //not sure if this is how you get stuff from another directory
  ifstream mfile;

  mfile.open(filename);

  if(!mfile.is_open()) return 1; // 1 is error code for, file didnt open

  // READ RLG327 MONSTER DESCRIPTION 1
  string met = "";
  mfile >> met;
  mfile.get();

  while(mfile) {
    string line;
    getline(input, line, ':'); // from example where file said UK Pop: 64000000, reads till : and then throws the : away

    int population;
    mfile >> population;

    mfile.get(); // takes the newline character and throws it away

    if(!input)  break; // ends the file read so that we dont print garbage

    cout << line << " -- " << population << endl; // prints UK Pop -- 64000000
                                                  //        France Pop -- 664000000
  }
  
  //READ THE MONSTERS

  // PRINT THE MONSTERS

  mfile.close();
  return 0;
}

int main(int argc, char *argv[]) {
    parseMonsters();
    return 0;
}
