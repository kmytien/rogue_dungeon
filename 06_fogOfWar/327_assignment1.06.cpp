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
  
  I THINK WE SHOULD SPLIT UP WORK AT THE BEGINNING/BE MORE CLEAR ABT WHAT PARTS WE ARE WORKING ON SO THAT IT IS EASIER TO CREATE AND THEN TEST CHUNKS ALSO SO WE DONT ALL END UP
  CODING STUFF THAT IS EITHER REPETETIVE OR CONTRADICTORY :) AND IF SOMEONE COMPLETES ONE PART THEY CAN HELP ANOTHER PERSON SINCE WE ALL WORK AT THE SAME TIME. LMK WHAT YALL
  THINK OF THIS :)) -H
**/
