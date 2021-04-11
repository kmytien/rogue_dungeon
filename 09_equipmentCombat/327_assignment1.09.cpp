/*
  NOTES:
  - edit pc
    - give pc a default num hitpoints
   H - give pc slots WEAPON, OFFHAND, RANGED, ARMOR, HELMET, CLOAK, GLOVES, BOOTS, AMULET, LIGHT, and two for RING
   H - ‘numbered’ a–l (that’s lowercase ’A’ through ’L’).
    - give pc 10 carry slots, numbered 0–9. (not effected by weapon/equip slots
    - When the PC walks over an item, if it has an open carry slot, it automatically picks the item up; else the item is ignored.
  S- add commands
    - w:  Wear an item. Prompts the user for a carry slot. If an item of that type is already equipped, items are swapped.
    - t Take off an item. Prompts for equipment slot. Item goes to an open carry slot.
    - d Drop an item. Prompts user for carry slot. Item goes to floor.
    - x Expunge an item from the game. Prompts the user for a carry slot. Item is permanently removed from the game.
    - i List PC inventory.
    - e List PC equipment.
    - I Inspect an item. Prompts user for a carry slot. Item’s description is displayed.
    - L Look at a monster. Enter a targeting mode similar to the controlled teleport in 1.06. Select a visible monster with t or abort with escape (there is no random). When
      a monster is selected, display its description (and other information, if you like). Escape will return back to normal input processing.
    ** handle failures "gracefully"
   - add bonuses to dice from equipment, ie all equipment dice are rolled and added to the characters dice when rolled (or not dice, like hp ints)
   - update combat
     - NPCs do not attack other NPCs. When an NPC attempts to move to a cell containing another NPC, the current occupant is displaced to any open cell neighboring 
       the occupant’s slot. If all neighboring cells are occupied, then displacement degenerates to a position swap.
     - Combat between PC and NPCs is initiated by attempts to move into the cell of the defensive character. The attack uses a character turn, so no move will occur, 
       only the attack.
     -  All attempted attacks connect
     - When an attack connects, the damage of the attack is calculated by rolling all applicable damage dice (NPCs have only one set; the PC has a base (bare-handed) 
       set which is rolled only if nothing is equipped in the weapon slot, to which is added rolls for all equipped items)
     - Character hitpoints are reduced by the calculated damage. If hitpoints fall below zero, the character dies and is removed from the game. As always, if the 
       PC dies, the game ends with a loss.
     - The game ends with a win if the PC kills an NPC with the BOSS flag (in our case, SpongeBob SquarePants). It no longer ends when the dungeon level is free of NPCs.

*/

// I GOT THESE 2 :) -H
void pc_equipables() {
}
void pc_inventory() {
}
