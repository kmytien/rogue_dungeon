#makefile for 1.04 pathfinding
run: 
	gcc heap.c heap.h rlg327.c 327_assignment1.04.c dungeon.c dungeon.h path.c path.h utils.c utils.h dims.h -o run
	
327_assignment1.04: 327_assignment1.04.c 327_assignment1.04.h
	gcc 327_assignment1.04.c -o 327_assignment1.04
	
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
	
clean:
	rm -f 327_assignment1.04 rlg327 heap run dungeon path utils dims
