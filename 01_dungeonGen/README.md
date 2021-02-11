# Assignment 1.01 | Dungeon Generation

- This assignment is for CS 327

- Here, we are creating and generating the dungeon with requirements listed below
  - All code is in C
  - Dungeon measures 80 units in the x (horizontal) direction and 21 units in the y (vertical) direction. A
  standard terminal is 80 × 24, and limiting the dungeon to 21 rows leaves three rows for text, things
  like gameplay messages and player status, which come later.
  - Require at least 6 rooms per dungeon
  - Each room measures at least 4 units in the x direction and at least 3 units in the y direction.
  - Rooms need not be rectangular, but neither may they contact one another. There must be at least 1
  cell of non-room between any two different rooms.
  - The outermost cells of the dungeon are immutable, thus they must remain rock and cannot be part of
  any room or corridor.
  - There should be at least one up and one down staircase. Staircases work like floor (for now) and
  should be placed in a location where floor would otherwise be. A character on the floor somewhere in
  the dungeon should be able to walk to the stairs.
  - Room cells should be drawn with periods, corridor cells with hashes, rock with spaces, up staircases
  with less-than signs, and down staircases with greater-than signs.
  - The dungeon should be fully connected, meaning that from any position on the floor, your adventurer
  should be able to walk to any other position on the floor without passing through rock.
  - Corridors should not extend into rooms, e.g., no hashes should be rendered inside rooms.
  
- Some parameters we've set for our dungeon generation
  - Can only generate 6 to 10 rooms
  - Rooms' length (x direction) can be 4 to 12 blocks
  - Rooms' height (y direction) can be 3 to 9 blocks


## Project Team Members:
- MyTien Kien *(kmytien)*
- Haylee Lawrence *(hayleel)*
- Sanjana Amatya *(samatya)*
