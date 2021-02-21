#include <stdio.h>
#include <stdlib.h>
#include "rlg327.c" //we really should have a header file, its the best practice
#include "heap.h" //we have a header for heap right?

/**
    Notes:
      - Need to use Djistka's Algorithm
      - Creating path finding algorithms for monsters
          - some can go through walls
          - some can only move through open space

      - Output: "Your submission, when run, should generate a dungeon, calculate all distance maps, render all three
        views of the dungeon (the “standard” view and the two distance maps), and exit."

      - Method: "A na¨ıve implementation will call pathfinding for every monster in the dungeon, but in practice, every
        monster is trying to get to the same place, so rather than calculating paths from the monsters to the player
        character (PC), we can instead calculate the distance from the PC to every point in the dungeon, and this
        only needs to be updated when the PC moves or the dungeon changes. Each monster will choose to move to
        the neighboring cell with the lowest distance to PC."

        - have a method that calcs distance to every point that calls dijkstrasMethod
        - printDungeon view that prints all three views

      - Probably should use his code for 1.01 & 1.02
**/


int main(int argc, char* argv[]) {
    dungeon_t *d;
    // generate a dungeon
    gen_dungeon(d);
    render_dungeon(d); // for testing purposes, pretty sure this is the print dungeon function

    // render pc at random spot 
    
    // calculate all distance maps

    // render all three views of the dungeon
    // standard

    // distance 1

    // distance 2
}


// method that calcs distance to every point that calls dijkstrasMethod
void gen_distances() {

    //generating distances for those who cannot tunnel (thru wall)


    //generating distances for those who can tunnel
    //  - have to use weights based on hardness

    
}


void dijkstras_method() {
  /** 
  //pseudo code
      function Dijkstra(Graph, source): //would graph be the 2d array of board? For tunnelling its the entire screen, for non its the floors? - i think so?? if im understanding u and the pdf correct
      dist[source] ← 0                           // Initialization

      create vertex priority queue Q

      for each vertex v in Graph:          
          if v ≠ source
              dist[v] ← INFINITY                 // Unknown distance from source to v
              prev[v] ← UNDEFINED                // Predecessor of v

              Q.add_with_priority(v, dist[v])


      while Q is not empty:                      // The main loop
          u ← Q.extract_min()                    // Remove and return best vertex
          for each neighbor v of u:              // only v that are still in Q
              alt ← dist[u] + length(u, v)
              if alt < dist[v]
                  dist[v] ← alt
                  prev[v] ← u
                  decrease_priority(v, alt)

      return dist, prev
  **/
}
