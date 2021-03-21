#need -lncurses to make
run: 
	g++ heap.c heap.h rlg327.c 327_assignment1.06.c dungeon.c dungeon.h path.c path.h utils.c utils.h dims.h character.c character.h event.c event.h move.c move.h npc.c npc.h pc.c pc.h -o run -lncurses
	
327_assignment1.06: 327_assignment1.06.cpp
	g++ 327_assignment1.06.cpp -o 327_assignment1.06
	
rlg327: rlg327.c
	g++ rlg327.c -o rlg327
	
heap: heap.c heap.h
	g++ heap.c -o heap
	
dungeon: dungeon.c dungeon.h
	g++ dungeon.c -o dungeon
	
path: path.c path.h
	g++ path.c -o path
	
utils: utils.c utils.h
	g++ utils.c -o utils
	
dims: dims.h
	g++ dims.h -o dims

character: character.c character.h
	g++ character.c -o character
  
event: event.c event.h
	g++ event.c -o event
  
move: move.c move.h
	g++ move.c -o move
  
npc: npc.c npc.h
	g++ npc.c -o npc
  
pc: pc.c pc.h
	g++ pc.c -o pc
	
clean:
	rm -f 327_assignment1.06 rlg327 heap run dungeon path utils dims character event move npc pc
