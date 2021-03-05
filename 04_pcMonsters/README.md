# Assignment 1.04 | Player Character & Monsters
- This assignment is for CS 327 Spring 2021
- We used Prof. Sheaffer's 1.03 assignment code for this assignment 

Please look at the certain parts of the assignment, we were not able to figure out the priority queue in run_turns() but are pretty sure move(), shortest_path() etc are working functions. 

-   *** may interpret these however we like ***
      - Need to move monsters around
          - "--nummon [INTEGER]" summons an INTEGER number of monsters around (10 is reasonable) DONE
          - Also have a hard coded default in case no --nummon
          - Represented with letters and occasionally letters/punctuation
          - Monsters may have 50% chance of having the following:
                - Intelligence: understands dungeon layout and moves on shortest path, can remember last known position
                  of previously seen PC
                - Unintelligent: monsters move in a straight line toward PC
                - Telepathy: always know where PC is, moves toward PC
                - Non-Telepathic: knows where PC is if PC is in line of sight, moves toward last known position
                - Tunneling Ability: can tunnel through rock
                - Erratic Behavior: have a 50% chance of moving as per their other characteristics,
                  otherwise they move to a random neighboring cell
      - Speed
        - Each monsters gets speed between 5 and 20
        - PC gets speed of 10
      - Basically whats supposed to happen in main(?) is
        - all monsters move (based on type) towards a (stationary) pc
        - when 2 characters land on the same space, the newcomer kills the original player on the cell
        - the game continues until the player dies or all the monsters are dead
        - (use usleep(3) which sleeps for an argument number of microseconds (250000ish) and when the game ends, prints win/lose status before exiting)

## Project Team Members:
- MyTien Kien *(kmytien)*
- Haylee Lawrence *(hayleel)*
- Sanjana Amatya *(samatya)*

