/**
  NOTES:
  - Convert everything to c++
    - make all files cpp files
    - create a c++ makefile
    - all major structs should be converted to classes
      - Make npc and pc inheritors of the character class
  - create another map that shows what the character has seen
    - character can see/remember a 3x3 radius around them 
    - this includes monsters (ie IF they are in the remembered areas)
  - add FOW and teleporting debug commands
    - use f to toggle fog of war view (whatever that means)
    - use g to toggle teleport mode
      - press g then g, the movement keys will move a targeted pointer (*) and the second g teleports the pc to that location
      - press g then r, sends the pc to a random location
      *** can be teleported into rock (not immutable), however, if SURROUNDED by rock, cannot tunnel (unless they teleport)
      
  ALL NEW CODE TO BE WRITTEN IN C++
  
  Files to be switched to C++ IM NOT SURE WHAT NEEDS TO BE CHANGED AND WHAT DOESNT (idk if the .h files need to be changed???)
        also none of the ones with DONE next to them have been tested, i can do that after everything is switched over :) -H 
  - makefile  DONE
  - character.c  DONE
  - character.h
  - dims.h
  - dungeon.c
  - dungeon.h
  - event.c
  - heap.c HEAP.C CAN STAY .C BUT WHAT ABT HEAP.H???
  - heap.h
  - io.c
  - io.h
  - move.c
  - move.h
  - npc.c
  - npc.h
  - path.c
  - path.h
  - pc.c
  - pc.h
  - rlg327.c
  - utils.c  DONE
  - utils.h
**/

/* CREATE NEW MAP */
// - create another map that shows what the character has seen
//     - character can see/remember a 3x3 radius around them 
//     - this includes monsters (ie IF they are in the remembered areas)

// updates new discovered_t dungeon
void update_discovered(dungeon_t *d) {
  //for 3x3 radius, update the discovered map MAKE SURE NOT TO DELETE REMEMBERED STUFF
}

// new render_dungeon USE THIS ONE FOR ALL EXCEPT WHEN TOGGLING FOW
void render_fov(dungeon_t *d) {
}

/* FOW COMMAND AND TELEPORTING COMMAND */
