#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>

#include "io.h"
#include "move.h"
#include "path.h"
#include "pc.h"
#include "utils.h"
#include "dungeon.h"
#include "object.h"
#include "npc.h"
#include "character.h"

/* Same ugly hack we did in path.c */
static dungeon *thedungeon;

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

void io_display_tunnel(dungeon *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (charxy(x, y) == d->PC) {
        mvaddch(y + 1, x, charxy(x, y)->symbol);
      } else if (hardnessxy(x, y) == 255) {
        mvaddch(y + 1, x, '*');
      } else {
        mvaddch(y + 1, x, '0' + (d->pc_tunnel[y][x] % 10));
      }
    }
  }
  refresh();
}

void io_display_distance(dungeon *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (charxy(x, y)) {
        mvaddch(y + 1, x, charxy(x, y)->symbol);
      } else if (hardnessxy(x, y) != 0) {
        mvaddch(y + 1, x, ' ');
      } else {
        mvaddch(y + 1, x, '0' + (d->pc_distance[y][x] % 10));
      }
    }
  }
  refresh();
}

static char hardness_to_char[] =
  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

void io_display_hardness(dungeon *d)
{
  uint32_t y, x;
  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      /* Maximum hardness is 255.  We have 62 values to display it, but *
       * we only want one zero value, so we need to cover [1,255] with  *
       * 61 values, which gives us a divisor of 254 / 61 = 4.164.       *
       * Generally, we want to avoid floating point math, but this is   *
       * not gameplay, so we'll make an exception here to get maximal   *
       * hardness display resolution.                                   */
      mvaddch(y + 1, x, (d->hardness[y][x]                             ?
                         hardness_to_char[1 + (int) ((d->hardness[y][x] /
                                                      4.2))] : ' '));
    }
  }
  refresh();
}

static void io_redisplay_visible_monsters(dungeon *d)
{
  /* This was initially supposed to only redisplay visible monsters.  After *
   * implementing that (comparitivly simple) functionality and testing, I   *
   * discovered that it resulted to dead monsters being displayed beyond    *
   * their lifetimes.  So it became necessary to implement the function for *
   * everything in the light radius.  In hindsight, it would be better to   *
   * keep a static array of the things in the light radius, generated in    *
   * io_display() and referenced here to accelerate this.  The whole point  *
   * of this is to accelerate the rendering of multi-colored monsters, and  *
   * it is *significantly* faster than that (it eliminates flickering       *
   * artifacts), but it's still significantly slower than it could be.  I   *
   * will revisit this in the future to add the acceleration matrix.        */
  pair_t pos;
  uint32_t color;
  uint32_t illuminated;

  for (pos[dim_y] = -PC_VISUAL_RANGE;
       pos[dim_y] <= PC_VISUAL_RANGE;
       pos[dim_y]++) {
    for (pos[dim_x] = -PC_VISUAL_RANGE;
         pos[dim_x] <= PC_VISUAL_RANGE;
         pos[dim_x]++) {
      if ((d->PC->position[dim_y] + pos[dim_y] < 0) ||
          (d->PC->position[dim_y] + pos[dim_y] >= DUNGEON_Y) ||
          (d->PC->position[dim_x] + pos[dim_x] < 0) ||
          (d->PC->position[dim_x] + pos[dim_x] >= DUNGEON_X)) {
        continue;
      }
      if ((illuminated = is_illuminated(d->PC,
                                        d->PC->position[dim_y] + pos[dim_y],
                                        d->PC->position[dim_x] + pos[dim_x]))) {
        attron(A_BOLD);
      }
      if (d->character_map[d->PC->position[dim_y] + pos[dim_y]]
                          [d->PC->position[dim_x] + pos[dim_x]] &&
          can_see(d, d->PC->position,
                  d->character_map[d->PC->position[dim_y] + pos[dim_y]]
                                  [d->PC->position[dim_x] +
                                   pos[dim_x]]->position, 1, 0)) {
        attron(COLOR_PAIR((color = d->character_map[d->PC->position[dim_y] +
                                                    pos[dim_y]]
                                                   [d->PC->position[dim_x] +
                                                    pos[dim_x]]->get_color())));
        mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                d->PC->position[dim_x] + pos[dim_x],
                character_get_symbol(d->character_map[d->PC->position[dim_y] +
                                                      pos[dim_y]]
                                                     [d->PC->position[dim_x] +
                                                      pos[dim_x]]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                          [d->PC->position[dim_x] + pos[dim_x]] &&
                 (can_see(d, d->PC->position,
                          d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                                   [d->PC->position[dim_x] +
                                    pos[dim_x]]->get_position(), 1, 0) ||
                 d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                          [d->PC->position[dim_x] + pos[dim_x]]->have_seen())) {
        attron(COLOR_PAIR(d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                                   [d->PC->position[dim_x] +
                                    pos[dim_x]]->get_color()));
        mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                d->PC->position[dim_x] + pos[dim_x],
                d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                         [d->PC->position[dim_x] + pos[dim_x]]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[d->PC->position[dim_y] + pos[dim_y]]
                                    [d->PC->position[dim_x] +
                                     pos[dim_x]]->get_color()));
      } else {
        switch (pc_learned_terrain(d->PC,
                                   d->PC->position[dim_y] + pos[dim_y],
                                   d->PC->position[dim_x] +
                                   pos[dim_x])) {
        case ter_wall:
        case ter_wall_immutable:
        case ter_unknown:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '.');
          break;
        case ter_floor_hall:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '#');
          break;
        case ter_debug:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '*');
          break;
        case ter_stairs_up:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '<');
          break;
        case ter_stairs_down:
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '>');
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          mvaddch(d->PC->position[dim_y] + pos[dim_y] + 1,
                  d->PC->position[dim_x] + pos[dim_x], '0');
        }
      }
      attroff(A_BOLD);
    }
  }

  refresh();
}

static int compare_monster_distance(const void *v1, const void *v2)
{
  const character *const *c1 = (const character *const *) v1;
  const character *const *c2 = (const character *const *) v2;

  return (thedungeon->pc_distance[(*c1)->position[dim_y]]
                                 [(*c1)->position[dim_x]] -
          thedungeon->pc_distance[(*c2)->position[dim_y]]
                                 [(*c2)->position[dim_x]]);
}

static character *io_nearest_visible_monster(dungeon *d)
{
  character **c, *n;
  uint32_t x, y, count, i;

  c = (character **) malloc(d->num_monsters * sizeof (*c));

  /* Get a linear list of monsters */
  for (count = 0, y = 1; y < DUNGEON_Y - 1; y++) {
    for (x = 1; x < DUNGEON_X - 1; x++) {
      if (d->character_map[y][x] && d->character_map[y][x] != d->PC) {
        c[count++] = d->character_map[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  thedungeon = d;
  qsort(c, count, sizeof (*c), compare_monster_distance);

  for (n = NULL, i = 0; i < count; i++) {
    if (can_see(d, character_get_pos(d->PC), character_get_pos(c[i]), 1, 0)) {
      n = c[i];
      break;
    }
  }

  free(c);

  return n;
}

void io_display(dungeon *d)
{
  pair_t pos;
  uint32_t illuminated;
  uint32_t color;
  character *c;
  int32_t visible_monsters;

  clear();
  for (visible_monsters = -1, pos[dim_y] = 0;
       pos[dim_y] < DUNGEON_Y;
       pos[dim_y]++) {
    for (pos[dim_x] = 0; pos[dim_x] < DUNGEON_X; pos[dim_x]++) {
      if ((illuminated = is_illuminated(d->PC,
                                        pos[dim_y],
                                        pos[dim_x]))) {
        attron(A_BOLD);
      }
      if (d->character_map[pos[dim_y]]
                          [pos[dim_x]] &&
          can_see(d,
                  character_get_pos(d->PC),
                  character_get_pos(d->character_map[pos[dim_y]]
                                                    [pos[dim_x]]), 1, 0)) {
        visible_monsters++;
        attron(COLOR_PAIR((color = d->character_map[pos[dim_y]]
                                                   [pos[dim_x]]->get_color())));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                character_get_symbol(d->character_map[pos[dim_y]]
                                                     [pos[dim_x]]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[pos[dim_y]]
                          [pos[dim_x]] &&
                 (d->objmap[pos[dim_y]]
                           [pos[dim_x]]->have_seen() ||
                  can_see(d, character_get_pos(d->PC), pos, 1, 0))) {
        attron(COLOR_PAIR(d->objmap[pos[dim_y]]
                                   [pos[dim_x]]->get_color()));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                d->objmap[pos[dim_y]]
                         [pos[dim_x]]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[pos[dim_y]]
                                    [pos[dim_x]]->get_color()));
      } else {
        switch (pc_learned_terrain(d->PC,
                                   pos[dim_y],
                                   pos[dim_x])) {
        case ter_wall:
        case ter_wall_immutable:
        case ter_unknown:
          mvaddch(pos[dim_y] + 1, pos[dim_x], ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '.');
          break;
        case ter_floor_hall:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '#');
          break;
        case ter_debug:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '*');
          break;
        case ter_stairs_up:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '<');
          break;
        case ter_stairs_down:
          mvaddch(pos[dim_y] + 1, pos[dim_x], '>');
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          mvaddch(pos[dim_y] + 1, pos[dim_x], '0');
        }
      }
      if (illuminated) {
        attroff(A_BOLD);
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d).",
           d->PC->position[dim_x], d->PC->position[dim_y]);
  mvprintw(22, 1, "%d known %s.", visible_monsters,
           visible_monsters > 1 ? "monsters" : "monster");
  mvprintw(22, 30, "Nearest visible monster: ");
  if ((c = io_nearest_visible_monster(d))) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at %d %c by %d %c.",
             c->symbol,
             abs(c->position[dim_y] - d->PC->position[dim_y]),
             ((c->position[dim_y] - d->PC->position[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->position[dim_x] - d->PC->position[dim_x]),
             ((c->position[dim_x] - d->PC->position[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

static void io_redisplay_non_terrain(dungeon *d, pair_t cursor)
{
  /* For the wiz-mode teleport, in order to see color-changing effects. */
  pair_t pos;
  uint32_t color;
  uint32_t illuminated;

  for (pos[dim_y] = 0; pos[dim_y] < DUNGEON_Y; pos[dim_y]++) {
    for (pos[dim_x] = 0; pos[dim_x] < DUNGEON_X; pos[dim_x]++) {
      if ((illuminated = is_illuminated(d->PC,
                                        pos[dim_y],
                                        pos[dim_x]))) {
        attron(A_BOLD);
      }
      if (cursor[dim_y] == pos[dim_y] && cursor[dim_x] == pos[dim_x]) {
        mvaddch(pos[dim_y] + 1, pos[dim_x], '*');
      } else if (d->character_map[pos[dim_y]][pos[dim_x]]) {
        attron(COLOR_PAIR((color = d->character_map[pos[dim_y]]
                                                   [pos[dim_x]]->get_color())));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                character_get_symbol(d->character_map[pos[dim_y]][pos[dim_x]]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[pos[dim_y]][pos[dim_x]]) {
        attron(COLOR_PAIR(d->objmap[pos[dim_y]][pos[dim_x]]->get_color()));
        mvaddch(pos[dim_y] + 1, pos[dim_x],
                d->objmap[pos[dim_y]][pos[dim_x]]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[pos[dim_y]][pos[dim_x]]->get_color()));
      }
      attroff(A_BOLD);
    }
  }

  refresh();
}

void io_display_no_fog(dungeon *d)
{
  uint32_t y, x;
  uint32_t color;
  character *c;

  clear();
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->character_map[y][x]) {
        attron(COLOR_PAIR((color = d->character_map[y][x]->get_color())));
        mvaddch(y + 1, x, character_get_symbol(d->character_map[y][x]));
        attroff(COLOR_PAIR(color));
      } else if (d->objmap[y][x]) {
        attron(COLOR_PAIR(d->objmap[y][x]->get_color()));
        mvaddch(y + 1, x, d->objmap[y][x]->get_symbol());
        attroff(COLOR_PAIR(d->objmap[y][x]->get_color()));
      } else {
        switch (mapxy(x, y)) {
        case ter_wall:
        case ter_wall_immutable:
          mvaddch(y + 1, x, ' ');
          break;
        case ter_floor:
        case ter_floor_room:
          mvaddch(y + 1, x, '.');
          break;
        case ter_floor_hall:
          mvaddch(y + 1, x, '#');
          break;
        case ter_debug:
          mvaddch(y + 1, x, '*');
          break;
        case ter_stairs_up:
          mvaddch(y + 1, x, '<');
          break;
        case ter_stairs_down:
          mvaddch(y + 1, x, '>');
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          mvaddch(y + 1, x, '0');
        }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d).",
           d->PC->position[dim_x], d->PC->position[dim_y]);
  mvprintw(22, 1, "%d %s.", d->num_monsters,
           d->num_monsters > 1 ? "monsters" : "monster");
  mvprintw(22, 30, "Nearest visible monster: ");
  if ((c = io_nearest_visible_monster(d))) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at %d %c by %d %c.",
             c->symbol,
             abs(c->position[dim_y] - d->PC->position[dim_y]),
             ((c->position[dim_y] - d->PC->position[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->position[dim_x] - d->PC->position[dim_x]),
             ((c->position[dim_x] - d->PC->position[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

void io_display_monster_list(dungeon *d)
{
  mvprintw(11, 33, " HP:    XXXXX ");
  mvprintw(12, 33, " Speed: XXXXX ");
  mvprintw(14, 27, " Hit any key to continue. ");
  refresh();
  getch();
}

uint32_t io_teleport_pc(dungeon *d)
{
  pair_t dest;
  int c;
  fd_set readfs;
  struct timeval tv;

  pc_reset_visibility(d->PC);
  io_display_no_fog(d);

  mvprintw(0, 0,
           "Choose a location.  'g' or '.' to teleport to; 'r' for random.");

  dest[dim_y] = d->PC->position[dim_y];
  dest[dim_x] = d->PC->position[dim_x];

  mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
  refresh();

  do {
    do{
      FD_ZERO(&readfs);
      FD_SET(STDIN_FILENO, &readfs);

      tv.tv_sec = 0;
      tv.tv_usec = 125000; /* An eigth of a second */

      io_redisplay_non_terrain(d, dest);
    } while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));
    /* Can simply draw the terrain when we move the cursor away, *
     * because if it is a character or object, the refresh       *
     * function will fix it for us.                              */
    switch (mappair(dest)) {
    case ter_wall:
    case ter_wall_immutable:
    case ter_unknown:
      mvaddch(dest[dim_y] + 1, dest[dim_x], ' ');
      break;
    case ter_floor:
    case ter_floor_room:
      mvaddch(dest[dim_y] + 1, dest[dim_x], '.');
      break;
    case ter_floor_hall:
      mvaddch(dest[dim_y] + 1, dest[dim_x], '#');
      break;
    case ter_debug:
      mvaddch(dest[dim_y] + 1, dest[dim_x], '*');
      break;
    case ter_stairs_up:
      mvaddch(dest[dim_y] + 1, dest[dim_x], '<');
      break;
    case ter_stairs_down:
      mvaddch(dest[dim_y] + 1, dest[dim_x], '>');
      break;
    default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
      mvaddch(dest[dim_y] + 1, dest[dim_x], '0');
    }
    switch ((c = getch())) {
    case '7':
    case 'y':
    case KEY_HOME:
      if (dest[dim_y] != 1) {
        dest[dim_y]--;
      }
      if (dest[dim_x] != 1) {
        dest[dim_x]--;
      }
      break;
    case '8':
    case 'k':
    case KEY_UP:
      if (dest[dim_y] != 1) {
        dest[dim_y]--;
      }
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      if (dest[dim_y] != 1) {
        dest[dim_y]--;
      }
      if (dest[dim_x] != DUNGEON_X - 2) {
        dest[dim_x]++;
      }
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      if (dest[dim_x] != DUNGEON_X - 2) {
        dest[dim_x]++;
      }
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      if (dest[dim_y] != DUNGEON_Y - 2) {
        dest[dim_y]++;
      }
      if (dest[dim_x] != DUNGEON_X - 2) {
        dest[dim_x]++;
      }
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      if (dest[dim_y] != DUNGEON_Y - 2) {
        dest[dim_y]++;
      }
      break;
    case '1':
    case 'b':
    case KEY_END:
      if (dest[dim_y] != DUNGEON_Y - 2) {
        dest[dim_y]++;
      }
      if (dest[dim_x] != 1) {
        dest[dim_x]--;
      }
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      if (dest[dim_x] != 1) {
        dest[dim_x]--;
      }
      break;
    }
  } while (c != 'g' && c != '.' && c != 'r');

  if (c == 'r') {
    do {
      dest[dim_x] = rand_range(1, DUNGEON_X - 2);
      dest[dim_y] = rand_range(1, DUNGEON_Y - 2);
    } while (charpair(dest) || mappair(dest) < ter_floor);
  }

  if (charpair(dest) && charpair(dest) != d->PC) {
    io_queue_message("Teleport failed.  Destination occupied.");
  } else {
    d->character_map[d->PC->position[dim_y]][d->PC->position[dim_x]] = NULL;
    d->character_map[dest[dim_y]][dest[dim_x]] = d->PC;

    d->PC->position[dim_y] = dest[dim_y];
    d->PC->position[dim_x] = dest[dim_x];
  }

  pc_observe_terrain(d->PC, d);
  dijkstra(d);
  dijkstra_tunnel(d);

  io_display(d);

  return 0;
}

/* Adjectives to describe our monsters */
static const char *adjectives[] = {
  "A menacing ",
  "A threatening ",
  "A horrifying ",
  "An intimidating ",
  "An aggressive ",
  "A frightening ",
  "A terrifying ",
  "A terrorizing ",
  "An alarming ",
  "A dangerous ",
  "A glowering ",
  "A glaring ",
  "A scowling ",
  "A chilling ",
  "A scary ",
  "A creepy ",
  "An eerie ",
  "A spooky ",
  "A slobbering ",
  "A drooling ",
  "A horrendous ",
  "An unnerving ",
  "A cute little ",  /* Even though they're trying to kill you, */
  "A teeny-weenie ", /* they can still be cute!                 */
  "A fuzzy ",
  "A fluffy white ",
  "A kawaii ",       /* For our otaku */
  "Hao ke ai de ",   /* And for our Chinese */
  "Eine liebliche "  /* For our Deutch */
  /* And there's one special case (see below) */
};

static void io_scroll_monster_list(char (*s)[60], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 9, " %-60s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static bool is_vowel(const char c)
{
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
          c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

static void io_list_monsters_display(dungeon *d,
                                     character **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[60]; /* pointer to array of 60 char */
  char tmp[41];  /* 19 bytes for relative direction leaves 40 bytes *
                  * for the monster's name (and one for null).      */

  (void) adjectives;

  s = (char (*)[60]) malloc((count + 1) * sizeof (*s));

  mvprintw(3, 9, " %-60s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 60, "You know of %d monsters:", count);
  mvprintw(4, 9, " %-60s ", s);
  mvprintw(5, 9, " %-60s ", "");

  for (i = 0; i < count; i++) {
    snprintf(tmp, 41, "%3s%s (%c): ",
             (is_unique(c[i]) ? "" :
              (is_vowel(character_get_name(c[i])[0]) ? "An " : "A ")),
             character_get_name(c[i]),
             character_get_symbol(c[i]));
    /* These pragma's suppress a "format truncation" warning from gcc. *
     * Stumbled upon a GCC bug when updating monster lists for 1.08.   *
     * Bug is known:                                                   *
     *    https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78969           *
     * GCC calculates a maximum length for the output string under the *
     * assumption that the int conversions can be 11 digits long (-2.1 *
     * billion).  The ints below can never be more than 2 digits.      *
     * Tried supressing the warning by taking the ints mod 100, but    *
     * GCC wasn't smart enough for that, so using a pragma instead.    */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
    snprintf(s[i], 60, "%40s%2d %s by %2d %s", tmp,
             abs(character_get_y(c[i]) - character_get_y(d->PC)),
             ((character_get_y(c[i]) - character_get_y(d->PC)) <= 0 ?
              "North" : "South"),
             abs(character_get_x(c[i]) - character_get_x(d->PC)),
             ((character_get_x(c[i]) - character_get_x(d->PC)) <= 0 ?
              "West" : "East"));
#pragma GCC diagnostic pop
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 9, " %-60s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 9, " %-60s ", "");
    mvprintw(count + 7, 9, " %-60s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 9, " %-60s ", "");
    mvprintw(20, 9, " %-60s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_monster_list(s, count);
  }

  free(s);
}

static void io_list_monsters(dungeon *d)
{
  character **c;
  uint32_t x, y, count;

  c = (character **) malloc(d->num_monsters * sizeof (*c));

  /* Get a linear list of monsters */
  for (count = 0, y = 1; y < DUNGEON_Y - 1; y++) {
    for (x = 1; x < DUNGEON_X - 1; x++) {
      if (d->character_map[y][x] && d->character_map[y][x] != d->PC &&
          can_see(d, character_get_pos(d->PC),
                  character_get_pos(d->character_map[y][x]), 1, 0)) {
        c[count++] = d->character_map[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  thedungeon = d;
  qsort(c, count, sizeof (*c), compare_monster_distance);

  /* Display it */
  io_list_monsters_display(d, c, count);
  free(c);

  /* And redraw the dungeon */
  io_display(d);
}

void io_handle_input(dungeon *d)
{
  uint32_t fail_code;
  int key;
  fd_set readfs;
  struct timeval tv;
  uint32_t fog_off = 0;
  pair_t tmp = { DUNGEON_X, DUNGEON_Y };

  do {
    do{
      FD_ZERO(&readfs);
      FD_SET(STDIN_FILENO, &readfs);

      tv.tv_sec = 0;
      tv.tv_usec = 125000; /* An eigth of a second */

      if (fog_off) {
        /* Out-of-bounds cursor will not be rendered. */
        io_redisplay_non_terrain(d, tmp);
      } else {
        io_redisplay_visible_monsters(d);
      }
    } while (!select(STDIN_FILENO + 1, &readfs, NULL, NULL, &tv));
    fog_off = 0;
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      fail_code = move_pc(d, 7);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      fail_code = move_pc(d, 8);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      fail_code = move_pc(d, 9);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      fail_code = move_pc(d, 6);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      fail_code = move_pc(d, 3);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      fail_code = move_pc(d, 2);
      break;
    case '1':
    case 'b':
    case KEY_END:
      fail_code = move_pc(d, 1);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      fail_code = move_pc(d, 4);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      fail_code = 0;
      break;
    case '>':
      fail_code = move_pc(d, '>');
      break;
    case '<':
      fail_code = move_pc(d, '<');
      break;
    case 'Q':
      d->quit = 1;
      fail_code = 0;
      break;
    case 'T':
      /* New command.  Display the distances for tunnelers.             */
      io_display_tunnel(d);
      fail_code = 1;
      break;
    case 'D':
      /* New command.  Display the distances for non-tunnelers.         */
      io_display_distance(d);
      fail_code = 1;
      break;
    case 'H':
      /* New command.  Display the hardnesses.                          */
      io_display_hardness(d);
      fail_code = 1;
      break;
    case 's':
      /* New command.  Return to normal display after displaying some   *
       * special screen.                                                */
      io_display(d);
      fail_code = 1;
      break;
    case 'g':
      /* Teleport the PC to a random place in the dungeon.              */
      io_teleport_pc(d);
      fail_code = 1;
      break;
    case 'f':
      io_display_no_fog(d);
      fail_code = 1;
      break;
     case 'm':
      io_list_monsters(d);
      fail_code = 1;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matterrs, but using this command will  *
       * waste a turn.  Set fail_code to 1 and you should be able to *
       * figure out why I did it that way.                           */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      fail_code = 0;
      break;

    //wear item and swap items
    case 'w':
      io_wear_item(d);
      break;

    //take off item
    case 't':
      io_remove_item(d);
      break;

    //drop item t floor
    case 'd':
      io_drop_item(d);
      break;

    //remove item from game
    case 'x':
      io_permanent_itemRemoval(d);
      break;

    //list pc inventory
    case 'i':
      io_display_inventory(d);
      break;

    //list pc equipment
    case 'e':
      io_display_equip(d);
      break;

    //inspect an item
    case 'I':
      io_inspect_item(d);
      break;

    //look at a monster
    case 'L':
      io_look_monster(d);
      break;

    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      fail_code = 1;
    }
  } while (fail_code);
}


//for equipment
const char* item_slot[] = {
  "weapon",
  "offhand",
  "ranged",
  "light",
  "armor",
  "helmet",
  "cloak",
  "gloves",
  "boots",
  "amulet",
  "lh ring",
  "rh ring"
};


//changes an object to a string
void io_convertObject(object *o, char *c, uint32_t size) {

  if (o) {
    //takes in all values as a string
    snprintf(c, size, "%s (speed: %d, damage: %d+%dd%d)",
    	o->get_name(), o->get_speed(), o->get_damage_base(), o->get_damage_number(), o->get_damage_sides());
  } else {
    *c = '\0';
  }

}


//wear item
uint32_t io_wear_item(dungeon *d){
    uint32_t i, pressKey;
    char c[80];
    int out = 0;

    //assuming it goes up to 10? was *inventory before
    //putItem is not a member in class pc**
    
    clear();
    for (i = 0; i < 10; i++) {
      io_convertObject(d->PC->inventory[i], c, 80);
      mvprintw(i + 5, 8, "%c) %s ", '0' + i, c);
    }

    mvprintw(17, 8, " %-60s ", "");
    mvprintw(18, 8, " %-60s ", "Select an item to wear. Press ESC to exit.");
    mvprintw(19, 8, " %-60s ", "");
    refresh();

    while (!out){
      if ((pressKey = getch()) == 27){
        io_display(d);
        out = 1;
      }

      if (pressKey < '0' || pressKey > '9') {
        mvprintw(18, 8, "%s ", "Numbers have to be between 0 and 9 (ESC to leave)");
      }
      
      if (!d->PC->inventory[pressKey - '0']){
      	mvprintw(18, 8, "Slot is empty. Please choose another slot.								");
      	continue;
      }

      if (!d->PC->pc_wear_item(d, pressKey - '0')){
        out = 1;
      }
			
      refresh();
    }
    
    clear();
    io_display(d);
    pc_stat_refresh(d);
    refresh();
    
    return 1;
}


// take item off -- case 't': use pc_remove_equipment
uint32_t io_remove_item(dungeon* d){

    // remove item
    uint32_t i, pressKey;
    char p[80];

		clear();
    for(i = 0; i < 12; i++) {
      io_convertObject(d->PC->equipment[i], p, 80);
      mvprintw(i + 5, 8, "%c [%s]\t) %s ", 'a' + i, item_slot[i], p);
    }

    mvprintw(17, 8, " %-60s ", "\n");
    mvprintw(18, 8, " %-60s ", "Which item do you want taken out? Press ESC to exit.");
    mvprintw(19, 8, " %-60s ", "\n");
    refresh();

    while (1) {
      if ((pressKey = getch()) == 27) {
        io_display(d);
        return 1;
      }

      if (pressKey < 'a' || pressKey > 'l') {
        mvprintw(18, 15, " %-60s ", "Letters have to be between a and l");
        refresh();
      }

      if (!d->PC->pc_remove_equipment(d, pressKey - 'a')){
        return 0;
      }

      mvprintw(18, 8, "Cannot remove item %s, please try again!", d->PC->equipment[pressKey - 'a']->get_name());
      refresh();
    }

		clear();
    io_display(d);
    pc_stat_refresh(d);
    refresh();
    return 1;
}


//drops an item on the floor -- case 'd': use pc_remove_inventory
uint32_t io_drop_item(dungeon *d) {
    int i, pressKey;
    char foo[80];

    for (i = 0; i < 10; i++) {
      mvprintw(i + 5, 8, " %c) %-60s ", '0' + i, d->PC->inventory[i] ? d->PC->inventory[i]->get_name() : "");
    }

    mvprintw(17, 8, " %-60s ", "");
    mvprintw(18, 8, " %-60s ", "Which item do you want dropped? Press ESC to exit.");
    mvprintw(19, 8, " %-60s ", "");
    refresh();

    while (1) {
      // have to use getch()
      if ((pressKey = getch()) == 27) {
        io_display(d);
        return 1;
      }

      if (pressKey < '0' || pressKey > '9') {
      	snprintf(foo, 80, "Invalid input: '%c'. Enter 0-9 or ESC to cancel.", pressKey);
        mvprintw(18, 8, " %-60s ", "Invalid entry. Must be keys 0-9.");
        refresh();
      }

      //NEED TO CHANGE THIS TO DROP
      if (!d->PC->pc_drop_equipment(d, pressKey - '0')) {
        return 1;
      }

      mvprintw(17, 8, "Cannot drop item %s, please try again!", d->PC->equipment[pressKey - '0']->get_name());
      mvprintw(18, 8, " %-60s ", "");
      refresh();
    }

		clear();
		io_display(d);
		pc_stat_refresh(d);
		refresh();
    return 1;
}


//permanently removes item from game
uint32_t io_permanent_itemRemoval(dungeon *d){

    uint32_t i, pressKey;

		clear();
    for(i = 0; i < 10; i++) {
      mvprintw(i + 5, 8, " %c) %-60s ", '0' + i, d->PC->inventory[i] ? d->PC->inventory[i]->get_name() : "");
    }

    mvprintw(17, 8, " %-60s ", "\n");
    mvprintw(18, 8, " %-60s ", "Which item do you want destroyed? Press ESC to exit.");
    mvprintw(19, 8, " %-60s ", "\n");
    mvprintw(20, 8, " %-60s ", "\n");
    refresh();

    while(1){
      if((pressKey = getch()) == 27){
        io_display(d);
        return 1;
      }

      if (pressKey < '0' || pressKey > '9') {
        mvprintw(18, 8, " %-60s ", "Numbers have to be between 0 and 9");
        refresh();
        continue;
      }
      
      if (!d->PC->inventory[pressKey - '0']) {
        mvprintw(18, 8, "Numbers have to be between 0-9, ESC to cancel.");
        refresh();
        continue;
      }

      if (!d->PC->pc_permanent_itemRemoval(d, pressKey - '0')) {
      	io_display(d);
        return 1;
      }

      mvprintw(18, 8, "Cannot destroy the item %s, please try again!", d->PC->equipment[pressKey - '0']->get_name());
      refresh();

    }
    
    clear();
    io_display(d);
    refresh();
    return 1;
}

//display inventory - key 'i'
void io_display_inventory(dungeon *d) {

  int i;
  char foo[80]; //prob can't use a char pointer? need to look into this
	
	clear();
  for (i = 0; i < 10; i++) {
    //need to display item name, speed, and damage
    io_convertObject(d->PC->inventory[i], foo, 80);
    mvprintw(i + 5, 8, "%d) %s", i, foo);
  }

  mvprintw(17, 8, " %s ", "");
  mvprintw(18, 8, "Hit any key to continue.        ");
  mvprintw(19, 8, " %s ", "");
  mvprintw(20, 8, " %s ", "");
  refresh();
  
  getch();
  io_display(d);
  refresh();
}

//displays what pc is wearing rn
void io_display_equip(dungeon *d) {

  int i;
  char foo[100]; //prob can't use a char pointer? need to look into this
  char alpha[] = "abcdefghijkl";

	clear();
  for (i = 0; i < 12; i++) {
    //need to display item name, speed, and damage
    io_convertObject(d->PC->equipment[i], foo, 100);
    mvprintw(i + 5, 8, "%c [%s]\t) %s", alpha[i], item_slot[i], foo);
  }

  mvprintw(17, 8, " %s", "");
  mvprintw(18, 8, "Hit any key to continue.        ");
  mvprintw(19, 8, " %s", "");
  mvprintw(20, 8, " %s", "");
  refresh();
  
  getch();
  io_display(d);
  refresh();
}


//inspects item - opens up list of items on hand, user press 0-9 depending on what item they want to inspect
//then that prints our the item name, speed, dmg, and the item description under it
void io_inspect_item(dungeon *d) {
  int i, key, out = 0;
  char foo[80]; //prob can't use a char pointer? need to look into this
  char output[80];
	
	clear();
  while (!out) {
  	for (i = 0; i < 10; i++) {
		  //need to display item name, speed, and damage
		  io_convertObject(d->PC->inventory[i], foo, 80);
		  mvprintw(i + 5, 8, "%d) %s", i, foo);
  	}

		mvprintw(18, 8, "%s", "What item would you like to inspect (0-9)? Press ESC to go back.");
		refresh();
  	
    switch (key = getch()) {
      case 48:
      case 49:
      case 50:
      case 51:
      case 52:
      case 53:
      case 54:
      case 55:
      case 56:
      case 57:
        //if empty
        if (!d->PC->inventory[key - '0']) {
          mvprintw(17, 8, "%s", "Slot picked is empty.																				");
          mvprintw(18, 8, "%s", "Hit any key to continue.");
					refresh();
					getch();
				}
        //else print name, speed, dmg and description
        else {
        	clear();
          //might need to make get_description for object
          io_convertObject(d->PC->inventory[key - '0'], output, 80);
          mvprintw(3, 0, "%s", output);
          mvprintw(4, 0, "%s", "");
          mvprintw(5, 0, "%s", d->PC->inventory[key - '0']->get_description());
          mvprintw(6, 0, "%s", "");
          mvprintw(15, 0, "%s", "Hit any key to continue.");
          refresh();
          getch();
          clear();
        }
        break;

      case 27:
        out = 1;
        break;

      default:
        mvprintw(17, 8, "%s", "Invalid key. Next time, use the keys 0-9.														");
        mvprintw(18, 8, "%s", "Hit any key to continue.");
			  refresh();
			  getch();
        break;
    }
    clear();
  }

	clear();
  io_display(d);
  refresh();
}


//looks at monsters - opens up the whole map similar to teleport mode, uses an asterisk to locate a monster,
//users oress t to select that monster and lists monster name, speed, hp, dmg, and description
//referenced off of teleport pc
void io_look_monster(dungeon *d) {

  int c, x, y, out = 0;
  char output1[80];
  pair_t current;
  current[dim_x] = d->PC->position[dim_x];
  current[dim_y] = d->PC->position[dim_y];

  //can only look at what is visible, can't open up no fog
  //mvprintw(1, 3, "%s", "Use the keys to find a monster. Press 't' to inspect the monster.");
  mvaddch(current[dim_y] + 1, current[dim_x], '*');
  refresh();

  while (!out) {
    mvprintw(1, 3, "%s", "Use the keys to find a monster. Press 't' to inspect and ESC to exit.");
		x = 0;
		y = 0;
		
    switch(c = getch()) {
      case '7':
      case 'y':
      case KEY_HOME:
        if (current[dim_y] != 1) y = -1;;
        if (current[dim_x] != 1) x = -1;;
        break;
      case '8':
      case 'k':
      case KEY_UP:
        if (current[dim_y] != 1) y = -1;
        x = 0;
        break;
      case '9':
      case 'u':
      case KEY_PPAGE:
        if (current[dim_y] != 1) y = -1;
        if (current[dim_x] != DUNGEON_X - 2) x = 1;
        break;
      case '6':
      case 'l':
      case KEY_RIGHT:
        if (current[dim_x] != DUNGEON_X - 2) x = 1;
        y = 0;
        break;
      case '3':
      case 'n':
      case KEY_NPAGE:
        if (current[dim_y] != DUNGEON_Y - 2) y = 1;
        if (current[dim_x] != DUNGEON_X - 2) x = 1;
        break;
      case '2':
      case 'j':
      case KEY_DOWN:
        if (current[dim_y] != DUNGEON_Y - 2) y = 1;
        x = 0;
        break;
      case '1':
      case 'b':
      case KEY_END:
        if (current[dim_y] != DUNGEON_Y - 2) y = 1;
        if (current[dim_x] != 1) x = -1;
        break;
      case '4':
      case 'h':
      case KEY_LEFT:
        if (current[dim_x] != 1) x = -1;
        y = 0;
        break;

      //print out description
      case 't':
        //would it be null if there is no character in certain spot?
        if (charpair(current) == NULL) {
        	mvprintw(1, 5, "%s", "No monster here, pick another spot.                                           "); 
        }

        else {
        	clear();
          snprintf(output1, 80, "%s: (speed: %d, damage: %d+%dd%d, hp: %d)",
                   charpair(current)->name, charpair(current)->speed,
                   charpair(current)->damage->get_base(), charpair(current)->damage->get_number(), 
                   charpair(current)->damage->get_sides(), charpair(current)->get_hp());

          mvprintw(3, 0, "%s", output1);
          mvprintw(4, 0, "%s", "");
          mvprintw(5, 0, "%s", ((npc *)charpair(current))->description);
          mvprintw(15, 0, "%s", "Hit any key to continue.");
        }

        refresh();
        getch();
        break;

      //ESC button to exit out
      case 27:
        out = 1;
        break;

      default:
        mvprintw(0, 5, "%s", "Invalid key.");
        break;
    }
    
    switch (mappair(current)) {
      case ter_wall:
      case ter_wall_immutable:
      case ter_unknown:
        mvaddch(current[dim_y] + 1, current[dim_x], ' ');
        break;
      case ter_floor:
      case ter_floor_room:
        mvaddch(current[dim_y] + 1, current[dim_x], '.');
        break;
      case ter_floor_hall:
        mvaddch(current[dim_y] + 1, current[dim_x], '#');
        break;
      case ter_debug:
        mvaddch(current[dim_y] + 1, current[dim_x], '*');
        break;
      case ter_stairs_up:
        mvaddch(current[dim_y] + 1, current[dim_x], '<');
        break;
      case ter_stairs_down:
        mvaddch(current[dim_y] + 1, current[dim_x], '>');
        break;
      default:
        mvaddch(current[dim_y] + 1, current[dim_x], '0');
    }
    
    current[dim_y] += y;
    current[dim_x] += x;
    
    io_display(d);
    mvaddch(current[dim_y] + 1, current[dim_x], '*');
    refresh();
  }

  refresh();
  io_display(d);
}
