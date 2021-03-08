#need -lncurses to make
run: 
	gcc heap.c heap.h rlg327.c 327_assignment1.04.c assignment_104.h dungeon.c dungeon.h path.c path.h utils.c utils.h dims.h character.c character.h event.c event.h move.c move.h npc.c npc.h pc.c pc.h -o run -lncurses
	
327_assignment1.05: 327_assignment1.05.c
	gcc 327_assignment1.05.c -o 327_assignment1.05
	
rlg327: rlg327.c
	gcc rlg327.c -o rlg327
	
heap: heap.c heap.h
	gcc heap.c -o heap
	
dungeon: dungeon.c dungeon.h
	gcc dungeon.c -o dungeon
	
path: path.c path.h
	gcc path.c -o path
	
utils: utils.c utils.h
	gcc utils.c -o utils
	
dims: dims.h
	gcc dims.h -o dims

character: character.c character.h
  gcc character.c -o character
  
event: event.c event.h
  gcc event.c -o event
  
move: move.c move.h
  gcc move.c -o move
  
npc: npc.c npc.h
  gcc npc.c -o npc
  
pc: pc.c pc.h
  gcc pc.c -o pc
	
clean:
	rm -f 327_assignment1.04 rlg327 heap run dungeon path utils dims character event move npc pc
