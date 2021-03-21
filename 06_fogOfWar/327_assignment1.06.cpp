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
  
  Files to be switched to C++ IM NOT SURE WHAT NEEDS TO BE CHANGED AND WHAT DOESNT
  - makefile
  - character.c 
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
  - utils.c
  - utils.h
**/
