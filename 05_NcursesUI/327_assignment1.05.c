#include stdio.h
#include ncurses.h

/**
  NOTES:
  - Make staircases functional
    - when a player goes up or down stairs generate a new dungeon and new monsters (up <, down >) WE SHOULD DO NON PERSISTANT/YOU CANNOT GO BACK TO THE SAME LEVEL
      - PC gets put in a random spot on the floor
  - Set commands NEVER NEED TO PRESS ENTER, A PRESSED KEY ACTIVATES COMMAND IMMEDIATELY, PRINT MESSAGES IF A COMMAND IS NOT POSSIBLE
    - <, > go up (<) or down (>) stairs if on a stair block ** NPCS CANNOT USE STAIRS
    - 7, y move PC 1 cell to the upper left
    - 8, k move PC 1 cell up
    - 9, u move PC 1 cell to upper right
    - 6, l move PC one cell to the right
    - 3, n move PC one cell to lower right
    - 2, j move PC one cell down
    - 1, b move PC one cell to the lower right
    - 4, h move PC one cell to the left
    - 5, [SPACE], . Rest for turn, NPCs still move
    - m display a list of monsters in the dungeon, with their symbol and position relative to the PC (e.g.: “c, 2 north and 14 west”).
    - [UP ARROW] when displaying monster list, if entire list does not fit in screen and not currently at top of list, scroll list up
    - [DOWN ARROW] when displaying monster list, if entire list does not fit in screen and not currently at bottom of list, scroll list down
    - [ESC] when displaying monster list, return back to player control
    - Q quit game
   ** DISPLAY IS LINES 1-21, USE 0 TO DISPLAY ERROR MESSAGES

**/

int main(
