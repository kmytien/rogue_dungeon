#need -lncurses to make
run: 
	g++ heap.c heap.h rlg327.cpp 327_assignment1.06.cpp dungeon.cpp dungeon.h path.cpp path.h utils.cpp utils.h dims.h character.cpp character.h event.cpp event.h move.cpp move.h npc.cpp npc.h pc.cpp pc.h -o run -lncurses
	
327_assignment1.06: 327_assignment1.06.cpppp
	g++ 327_assignment1.06.cpppp -o 327_assignment1.06
	
rlg327: rlg327.cpp
	g++ rlg327.cpp -o rlg327
	
heap: heap.c heap.h
	g++ heap.c -o heap
	
dungeon: dungeon.cpp dungeon.h
	g++ dungeon.cpp -o dungeon
	
path: path.cpp path.h
	g++ path.cpp -o path
	
utils: utils.cpp utils.h
	g++ utils.cpp -o utils
	
dims: dims.h
	g++ dims.h -o dims

character: character.cpp character.h
	g++ character.cpp -o character
  
event: event.cpp event.h
	g++ event.cpp -o event
  
move: move.cpp move.h
	g++ move.cpp -o move
  
npc: npc.cpp npc.h
	g++ npc.cpp -o npc
  
pc: pc.cpp pc.h
	g++ pc.cpp -o pc
	
clean:
	rm -f 327_assignment1.06 rlg327 heap run dungeon path utils dims character event move npc pc
