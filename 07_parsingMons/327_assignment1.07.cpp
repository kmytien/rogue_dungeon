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

//monster descriptions class
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

    //printing monsters
    void print_monsters(){
        cout << name << desc << symbol << color << speed << ability << hp << damage << rrty << endl;

        //resetting monster description variables
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


//reading from file and filling in the monster desc. variables
int parse_monsters() {

    string filename;
    filename = getenv("HOME");
    filename += "/.rlg327/monster_desc.txt";

    ifstream mfile;
    monster_desc md;
    string s = "";
    char c;

    //opening file, checks if file is opened
    mfile.open(filename);
    if (!mfile.is_open()) {
        cout << "Cannot open file." << endl;
        return -1;
    }

    //checking for first line - "RLG327 MONSTER DESCRIPTION 1", then skipping new line
    getline(mfile, s, '\n');
  	mfile.get();

    if (s != "RLG327 MONSTER DESCRIPTION 1") {
        cout << "File is in wrong format." << endl;
        return -1;
    }


    //while we are not at the EOF
    while (mfile) {

        //looking for "BEGIN MONSTER"
        getline(mfile, s, '\n');
        if (s != "BEGIN MONSTER") break;

        //peeks at next beginning character
    	c = mfile.peek();

        //while the next beginnign character is NOT "E" - so if next word is NOT "END"
    	while (c != 'E') {

            //continues if next character is a newline or a period
      	    if (c == '\n' || c == '.') continue;

      		  //checking for name
      		  if (c == 'N') {
        	  	getline(mfile, s, ' ');
        		getline(mfile, s, '\n');
        		md.name = s + "\n";
      	 	  }

      	 	  //checking for symbol and speed
      	 	  else if (c == 'S') {
        	 	getline(mfile, s, ' ');

                	//case for symbol
        		 if (s == "SYMB") {
          		        getline(mfile, s, '\n');
          		        md.symbol = s + "\n";
        		 }

                	//case for speed
        		else if (s == "SPEED") {
          			getline(mfile, s, '\n');
          			md.speed = s + "\n";
        		}
      		  }

      		  //checking for color
      		  else if (c == 'C') {
        		getline(mfile, s, ' ');
        		getline(mfile, s, '\n');
        		md.color = s + "\n";
      		  }

      		  //checking for description or damage
      		  else if (c == 'D') {
        		mfile.get(); //D is extracted
        		mfile.get(c); //whatever next letter is extracted

                	//case for description
        		if (c == 'E') {
          		        getline(mfile, s, '\n');
          	                getline(mfile, s, '\n');

          		        while (s != ".") {
          				md.desc += s + "\n";
          				getline(mfile, s, '\n');
          		        }
        	  	}

                	//case for damage
        		else if (c == 'A') {
          			getline(mfile, s, ' ');
          			getline(mfile, s, '\n');
          			md.damage = s + "\n";
        		}
      		  }

      		  //checking for hp
      		  else if (c == 'H') {
        		getline(mfile, s, ' ');
        		getline(mfile, s, '\n');
        		md.hp = s + "\n";
      		  }

      		  //checking for rarity
      		  else if (c == 'R') {
        		getline(mfile, s, ' ');
       			getline(mfile, s, '\n');
        		md.rrty = s + "\n";
      		  }

      		  //checking for abilities - check if parses more than one word
      		  else if (c == 'A') {
        		getline(mfile, s, ' ');
        		getline(mfile, s, '\n');
        		md.ability = s + "\n";
      		  }

            	  //peeking at next letter in next line
      		  c = mfile.peek();
      	}

        //checking for END
        getline(mfile, s, '\n');
        if (s != "END") break;
        md.print_monsters();

        //skips the newline before "BEGIN MONSTER" and starts over
        mfile.get();
        }

    mfile.close();
    return 0;
}

//main method
int main(int argc, char *argv[]) {
    parse_monsters();
    return 0;
}
