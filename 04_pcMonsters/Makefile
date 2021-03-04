#makefile for 1.04 pcMonsters
run: 
	gcc heap.c heap.h rlg327.c rlg327.h 327_assignment1.04.c -o run
	
rlg327: rlg327.c
	gcc rlg327.c -o rlg327
  
327_assignment1.04: 327_assignment1.04.c 327_assignment1.04.h
	gcc 327_assignment1.03.c -o 327_assignment1.03
	
heap: heap.c heap.h
	gcc heap.c -o heap
	
clean:
	rm -f 327_assignment1.03 rlg327 heap run
