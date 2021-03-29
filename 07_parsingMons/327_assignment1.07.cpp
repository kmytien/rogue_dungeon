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
#include <string>

using namespace std;

/*class dice {
public:
    int base;
    int sides;
    int number;
};*/

class monster_desc {
public:
    string name;
    string desc;
    string color;
    string ability;
    string speed;
    string hp;
    string damage;
    string symbol;
    string rrty;

    void print_monsters(){
        // PRINT THE MONSTERS
        cout << name << endl;
        cout << symbol << endl;
        cout << color << endl;
        cout << desc << "." << endl;
        cout << speed << endl;
        cout << hp << endl;
        cout << damage << endl;
        cout << rrty << endl;
        cout << ability << endl;
        cout << "\n\n" << endl;

        name.erase();
        desc.erase();
        color.erase();
        ability.erase();
        speed.erase();
        hp.erase();
        damage.erase();
        symbol.erase();
        rrty.erase();
    }
};


int parse_monsters() {

  string filename;
  filename = getenv("HOME");
  filename += "/.rlg327/monster_desc.txt"; //not sure if this is how you get stuff from another directory

  ifstream mfile;
  monster_desc md;
  string s = "";

  mfile.open(filename);

  if (!mfile.is_open()) {
      cout << "Cannot open file." << endl;
      return 1; // 1 is error code for, file didnt open
  }

  // READ RLG327 MONSTER DESCRIPTION 1 AND IF IT FAILS TO MATCH, EXIT WITH ERROR CODE -1 code has been tested :) -h
  //s = "";

  getline(mfile, s, '\n');
  mfile.get();

  if (s != "RLG327 MONSTER DESCRIPTION 1") {
      cout << "File is in wrong format." << endl;
      return -1; //can we compare strings like this? i thought we used .compare()
  }

  //read the data
  while (mfile) {
      // BEGIN MONSTER
      s = "";
      getline(mfile, s, '\n');
      if (s != "BEGIN MONSTER") break; //HOPEFULLY THIS GOES TO THE NEXT BEGIN MONSTER??

      // NAME Junior Barbarian
      s = "";
      getline(mfile, s, ' ');
      if (s != "NAME") break;
      getline(mfile, s, '\n');
      md.name = s;

      // SYMB p
      s = "";
      getline(mfile, s, '\n');
      if (s != "SYMB") break;
      getline(mfile, s, '\n');
      md.symbol = s;

      // COLOR BLUE
      s = "";
      getline(mfile, s, ' ');
      if (s != "COLOR") break;
      getline(mfile, s, '\n');
      md.color = s;

      // DESC
      // This is a junior barbarian. He--or is it she? You can't tell for sure--
      // looks like... it should still be in barbarian school. The barbarians are
      // putting them in the dungeons young these days. It's wearing dirty, tattered
      // cloth armor and wielding a wooden sword. You have a hard time feeling
      // intimidated.
      // .
      s = "";
      getline(mfile, s, '\n'); // not sure about this
      if (s != "DESC") break;
      getline(mfile, s, '.');
      mfile.get();
      md.desc = s;

      // SPEED 7+1d4
      s = "";
      getline(mfile, s, ' ');
      if (s != "SPEED") break;
      getline(mfile, s, '\n');
      md.speed = s;

      // DAM 0+1d4
      s = "";
      getline(mfile, s, ' ');
      if (s != "DAM") break;
      getline(mfile, s, '\n');
      md.damage = s;

      // HP 12+2d6
      s = "";
      getline(mfile, s, ' ');
      if (s != "HP") break;
      getline(mfile, s, '\n');
      md.hp = s;

      // RRTY 100
      s = "";
      getline(mfile, s, ' ');
      if (s != "RRTY") break;
      getline(mfile, s, '\n');
      md.rrty = s;

      // ABIL SMART
      s = "";
      getline(mfile, s, ' ');
      if (s != "ABIL") break;
      getline(mfile, s, '\n');
      md.ability = s;

      // END
      s = "";
      getline(mfile, s, '\n');
      if (s != "END") break;

      md.print_monsters();
  }

  mfile.close();
  return 0;
}

int main(int argc, char *argv[]) {
    parse_monsters();
    return 0;
}

//  EXAMPLE FROM VIDEO ON PARSING
//
// while(mfile) {
//   string line;
//   getline(input, line, ':'); // from example where file said UK Pop: 64000000, reads till : and then throws the : away
//
//   int population;
//   mfile >> population;
//
//   mfile.get(); // takes the newline character and throws it away
//
//   if(!input)  break; // ends the file read so that we dont print garbage
//
//   cout << line << " -- " << population << endl; // prints UK Pop -- 64000000
// }                                              //         France Pop -- 664000000
