#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include <sys/stat.h>
#include <cstring>
#include <string>

/**
  *	COMS 327: Assignment 1.07 - Parsing Monster Desc.
  * By: Sanjana Amatya, MyTien Kien, Haylee Lawrence
  *
**/  

using namespace std;

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
        cout << name << desc << symbol << color << speed << ability << hp << damage << rrty << endl;

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
  char c;
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

  getline(mfile, s, '\n');
	mfile.get(c);
	
  if (s != "RLG327 MONSTER DESCRIPTION 1") {
      cout << "File is in wrong format." << endl;
      return -1; 
  }
	
	
  //read the data
  while (1) {
  	
    // BEGIN MONSTER
    getline(mfile, s, '\n');
    cout << s << endl;
    if (s != "BEGIN MONSTER") break; //HOPEFULLY THIS GOES TO THE NEXT BEGIN MONSTER??
		c = mfile.peek();
		
	  while (c != 'E') {
	  	
	  	if (c == '\n' || c == '.') continue;
	  	
		  //NAME Junior Barbarian
		  if (c == 'N') {
			  getline(mfile, s, ' ');
			  getline(mfile, s, '\n');
			  md.name = s + "\n";
	 	  }
	 
	 		//SYMBOL & SPEED
	 	  else if (c == 'S') {
	 	  	getline(mfile, s, ' ');
	 	  	
	 	  	if (s == "SYMB") { //symbol
					getline(mfile, s, '\n');
					md.symbol = s + "\n";
			  }
			  
			  else if (s == "SPEED") { //speed
					getline(mfile, s, '\n');
					md.speed = s + "\n";
			  }
		  }

		  //COLOR BLUE
		  else if (c == 'C') {
				getline(mfile, s, ' ');
				getline(mfile, s, '\n');
				md.color = s + "\n";
			}

		  //DESC
		  else if (c == 'D') {
		  	mfile.get(); //D is extracted
		  	mfile.get(c); //whatever next letter is extracted
		  	
		  	if (c == 'E') { //desc
					getline(mfile, s, '\n');
					getline(mfile, s, '\n');
					
					while (s != ".") {
						md.desc += s + "\n";
						getline(mfile, s, '\n');
					}
				}
				
				else if (c == 'A') { //dam
					getline(mfile, s, ' ');
					getline(mfile, s, '\n');
					md.damage = s + "\n";
				}
		  }

		  //HP 12+2d6
		  else if (c == 'H') {
				getline(mfile, s, ' ');
				getline(mfile, s, '\n');
				md.hp = s + "\n";
			}

		  //RRTY 100
		  else if (c == 'R') {
				getline(mfile, s, ' ');
				getline(mfile, s, '\n');
				md.rrty = s + "\n";
		  }
		  
		  //ABIL SMART
		  else if (c == 'A') {
				getline(mfile, s, ' ');
				getline(mfile, s, '\n');
				md.ability = s + "\n";
		  }
		  
		  c = mfile.peek();
	  }
		
    //END
    getline(mfile, s, '\n');
    if (s != "END") break;
    md.print_monsters();
    
    mfile.get();
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
