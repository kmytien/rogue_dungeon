#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#include "heap.h"
#include "rlg327.h"

static int32_t dijkstra_nontunnel_cmp(const void *key, const void *with) {
    return ((dijkstra_nontunnel_t *) key)->cost - ((dijkstra_nontunnel_t *) with)->cost;
}

//For non-tunneling monsters
static void dijkstra_nontunnel(dungeon_t *d) {

    static monster_path_t path[DUNGEON_Y][DUNGEON_X], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y, i, j;

    if (!initialized) {
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                path[y][x].pos[dim_y] = y;
                path[y][x].pos[dim_x] = x;
            }
        }
        initialized = 1;
    }


    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            path[y][x].cost = INT_MAX;
            d->nt_dist[y][x] = INT_MAX;
        }
    }


    path[d->pc[dim_y]][d->pc[dim_x]].cost = 0;
    heap_init(&h, dijkstra_nontunnel_cmp, NULL);


    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (hardnessxy(x, y) == 0) {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
            } else {
                path[y][x].hn = NULL;
            }
        }
    }


    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;


        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
        }

        if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
        }

        if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > (p->cost + tunnel_cost(p->pos)))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
        }
    }

    heap_delete(&h);


    //set the costs to the distance map
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            d->nt_dist[i][j] = path[i][j].cost;
        }
    }


    pair_t pa;

    for (pa[dim_y] = 0; pa[dim_y] < DUNGEON_Y; pa[dim_y]++) {
        for (pa[dim_x] = 0; pa[dim_x] < DUNGEON_X; pa[dim_x]++) {

            if (pa[dim_x] == d->pc[dim_x] && pa[dim_y] == d->pc[dim_y])
                printf("%c", '@');

            else {
                if (mappair(pa) == ter_wall_immutable || mappair(pa) == ter_wall)
                    printf("%c", ' ');

                else if (mappair(pa) == ter_floor || mappair(pa) == ter_floor_room || mappair(pa) == ter_floor_hall
                         || mappair(pa) == ter_stairs || mappair(pa) == ter_stairs_up ||
                         mappair(pa) == ter_stairs_down) {

                    if (path[pa[dim_y]][pa[dim_x]].cost == INT_MAX)
                        printf("%c", 'X');

                    else
                        printf("%d", path[pa[dim_y]][pa[dim_x]].cost % 10);

                }
            }
        }
        printf("\n");
    }

}


static int32_t dijkstra_tunnel_cmp(const void *key, const void *with) {
    return ((dijkstra_tunnel_t *) key)->cost - ((dijkstra_tunnel_t *) with)->cost;
}


//For tunnelling monsters
static void dijkstra_tunnel(dungeon_t *d) {

    static monster_path_t path[DUNGEON_Y][DUNGEON_X], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y, i, j;

    if (!initialized) {
        for (y = 0; y < DUNGEON_Y; y++) {
            for (x = 0; x < DUNGEON_X; x++) {
                path[y][x].pos[dim_y] = y;
                path[y][x].pos[dim_x] = x;
            }
        }
        initialized = 1;
    }


    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            path[y][x].cost = INT_MAX;
            d->t_dist[y][x] = INT_MAX;
        }
    }


    path[d->pc[dim_y]][d->pc[dim_x]].cost = 0;
    heap_init(&h, dijkstra_tunnel_cmp, NULL);


    for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            if (mapxy(x, y) != ter_wall_immutable) {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
            } else {
                path[y][x].hn = NULL;
            }
        }
    }


    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
        }

        if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
        }

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
        }

        if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
        }

        if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > p->cost + tunnel_cost(p->pos))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = p->cost + tunnel_cost(p->pos);
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
        }
    }

    heap_delete(&h);


    //set the cost equal to the map cells
    for (i = 0; i < DUNGEON_Y; i++) {
        for (j = 0; j < DUNGEON_X; j++) {
            d->t_dist[i][j] = path[i][j].cost;
        }
    }


    pair_t pa;

    for (pa[dim_y] = 0; pa[dim_y] < DUNGEON_Y; pa[dim_y]++) {
        for (pa[dim_x] = 0; pa[dim_x] < DUNGEON_X; pa[dim_x]++) {

            if (pa[dim_x] == d->pc[dim_x] && pa[dim_y] == d->pc[dim_y])
                printf("%c", '@');

            else {
                if (mappair(pa) == ter_wall_immutable)
                    printf("%c", ' ');

                else if (mappair(pa) == ter_floor || mappair(pa) == ter_floor_room || mappair(pa) == ter_floor_hall
                         || mappair(pa) == ter_stairs || mappair(pa) == ter_stairs_up ||
                         mappair(pa) == ter_stairs_down || mappair(pa) == ter_wall) {

                    if (path[pa[dim_y]][pa[dim_x]].cost == INT_MAX)
                        printf("%c", 'X');

                    else
                        printf("%d", path[pa[dim_y]][pa[dim_x]].cost % 10);

                }
            }
        }
        printf("\n");
    }
}


int main(int argc, char *argv[]) {

    dungeon_t d;
    time_t seed;
    struct timeval tv;
    uint32_t i;
    uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
    uint32_t long_arg;
    char *save_file;
    char *load_file;
    char *pgm_file;

    //UNUSED(in_room); /* Suppress warning */

    /* Default behavior: Seed with the time, generate a new dungeon, *
     * and don't write to disk.                                      */
    do_load = do_save = do_image = do_save_seed = do_save_image = 0;
    do_seed = 1;
    save_file = load_file = NULL;

    /* The project spec requires '--load' and '--save'.  It's common  *
     * to have short and long forms of most switches (assuming you    *
     * don't run out of letters).  For now, we've got plenty.  Long   *
     * forms use whole words and take two dashes.  Short forms use an *
      * abbreviation after a single dash.  We'll add '--rand' (to     *
     * specify a random seed), which will take an argument of it's    *
     * own, and we'll add short forms for all three commands, '-l',   *
     * '-s', and '-r', respectively.  We're also going to allow an    *
     * optional argument to load to allow us to load non-default save *
     * files.  No means to save to non-default locations, however.    *
     * And the final switch, '--image', allows me to create a dungeon *
     * from a PGM image, so that I was able to create those more      *
     * interesting test dungeons for you.                             */

    if (argc > 1) {
        for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
            if (argv[i][0] == '-') { /* All switches start with a dash */
                if (argv[i][1] == '-') {
                    argv[i]++;    /* Make the argument have a single dash so we can */
                    long_arg = 1; /* handle long and short args at the same place.  */
                }
                switch (argv[i][1]) {
                    case 'r':
                        if ((!long_arg && argv[i][2]) ||
                            (long_arg && strcmp(argv[i], "-rand")) ||
                            argc < ++i + 1 /* No more arguments */ ||
                            !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
                            usage(argv[0]);
                        }
                        do_seed = 0;
                        break;
                    case 'l':
                        if ((!long_arg && argv[i][2]) ||
                            (long_arg && strcmp(argv[i], "-load"))) {
                            usage(argv[0]);
                        }
                        do_load = 1;
                        if ((argc > i + 1) && argv[i + 1][0] != '-') {
                            /* There is another argument, and it's not a switch, so *
                             * we'll treat it as a save file and try to load it.    */
                            load_file = argv[++i];
                        }
                        break;
                    case 's':
                        if ((!long_arg && argv[i][2]) ||
                            (long_arg && strcmp(argv[i], "-save"))) {
                            usage(argv[0]);
                        }
                        do_save = 1;
                        if ((argc > i + 1) && argv[i + 1][0] != '-') {
                            /* There is another argument, and it's not a switch, so *
                             * we'll save to it.  If it is "seed", we'll save to    *
                         * <the current seed>.rlg327.  If it is "image", we'll  *
                         * save to <the current image>.rlg327.                  */
                            if (!strcmp(argv[++i], "seed")) {
                                do_save_seed = 1;
                                do_save_image = 0;
                            } else if (!strcmp(argv[i], "image")) {
                                do_save_image = 1;
                                do_save_seed = 0;
                            } else {
                                save_file = argv[i];
                            }
                        }
                        break;
                    case 'i':
                        if ((!long_arg && argv[i][2]) ||
                            (long_arg && strcmp(argv[i], "-image"))) {
                            usage(argv[0]);
                        }
                        do_image = 1;
                        if ((argc > i + 1) && argv[i + 1][0] != '-') {
                            /* There is another argument, and it's not a switch, so *
                             * we'll treat it as a save file and try to load it.    */
                            pgm_file = argv[++i];
                        }
                        break;
                    default:
                        usage(argv[0]);
                }
            } else { /* No dash */
                usage(argv[0]);
            }
        }
    }

    if (do_seed) {
        /* Allows me to generate more than one dungeon *
         * per second, as opposed to time().           */
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    printf("Seed is %ld.\n", seed);
    srand(seed);

    init_dungeon(&d);

    d.pc[dim_x] = d.pc[dim_y] = 0; /* Suppress false positive from Valgrind */

    if (do_load) {
        read_dungeon(&d, load_file);
    } else if (do_image) {
        read_pgm(&d, pgm_file);
    } else {
        gen_dungeon(&d);
    }

    if (!do_load) {
        /* Set a valid position for the PC */
        d.pc[dim_x] = d.rooms[0].position[dim_x];
        d.pc[dim_y] = d.rooms[0].position[dim_y];
    }

    render_dungeon(&d);
    dijkstra_nontunnel(&d);
    dijkstra_tunnel(&d);

    if (do_save) {
        if (do_save_seed) {
            /* 10 bytes for number, plus dot, extention and null terminator. */
            save_file = malloc(18);
            sprintf(save_file, "%ld.rlg327", seed);
        }
        if (do_save_image) {
            if (!pgm_file) {
                fprintf(stderr, "No image file was loaded.  Using default.\n");
                do_save_image = 0;
            } else {
                /* Extension of 3 characters longer than image extension + null. */
                save_file = malloc(strlen(pgm_file) + 4);
                strcpy(save_file, pgm_file);
                strcpy(strchr(save_file, '.') + 1, "rlg327");
            }
        }
        write_dungeon(&d, save_file);

        if (do_save_seed || do_save_image) {
            free(save_file);
        }
    }

    delete_dungeon(&d);

    return 0;
}
