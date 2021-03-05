#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#include "dungeon.h"
#include "path.h"
#include "assignment_104.h"


void usage(char *name)
{
    fprintf(stderr,
            "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
            "          [-s|--save [<file>]] [-i|--image <pgm file>]\n",
            name);

    exit(-1);
}

int main(int argc, char *argv[])
{
    dungeon_t d;
    time_t seed;
    struct timeval tv;
    uint32_t i;
    uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image, do_nummon;
    uint32_t long_arg;
    char *save_file;
    char *load_file;
    char *pgm_file;


    /* Quiet a false positive from valgrind. */
    memset(&d, 0, sizeof (d));

    /* Default behavior: Seed with the time, generate a new dungeon, *
     * and don't write to disk.                                      */
    do_load = do_save = do_image = do_save_seed = do_save_image = do_nummon = 0;
    do_seed = 1;
    save_file = load_file = NULL;

    if (argc > 1) {
        for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
            if (argv[i][0] == '-') { /* All switches start with a dash */
                if (argv[i][1] == '-') {
                    argv[i]++;    /* Make the argument have a single dash so we can */
                    long_arg = 1; /* handle long and short args at the same place.  */
                }
                switch (argv[i][1]) {
                    case 'n':
                        if ((!long_arg && argv[i][2]) ||
                            (long_arg && strcmp(argv[i], "-nummon")) ||
                            argc < ++i + 1 /* No more arguments */) {
                            usage(argv[0]);
                        }
                        do_nummon = 1;
                        break;
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


    if (do_load) {
        read_dungeon(&d, load_file);
    } else if (do_image) {
        read_pgm(&d, pgm_file);
    } else if (do_nummon) {
        printf("sdf");
        d.num_monsters = (uint32_t) atoi(argv[2]);
    } else {
        gen_dungeon(&d);
        d.num_monsters = 11;
    }

    if (!do_load) {
        /* Set a valid position for the PC */
        d.pc.position[dim_x] = (d.rooms[0].position[dim_x] +
                                (rand() % d.rooms[0].size[dim_x]));
        d.pc.position[dim_y] = (d.rooms[0].position[dim_y] +
                                (rand() % d.rooms[0].size[dim_y]));
    }

    printf("PC is at (y, x): %d, %d\n",
           d.pc.position[dim_y], d.pc.position[dim_x]);
printf("sdjfs");
    // dijkstra(&d);
    // dijkstra_tunnel(&d);
    run_turns(&d);

//   render_dungeon(&d);

//   dijkstra(&d);   dijkstra_tunnel(&d);
//   render_distance_map(&d);
//   render_tunnel_distance_map(&d);
//   render_hardness_map(&d);
//   render_movement_cost_map(&d);

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
