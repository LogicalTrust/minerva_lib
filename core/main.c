/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>

#include <minerva_colors.h>
#include <minerva_var.h>
#include <minerva_call.h>
#include <minerva_func.h>
#include <minerva_parser.h>
#include <minerva_dict.h>
#include <minerva_repl.h>
#include <minerva_trace.h>
#include <target.h>
#include <xmalloc.h>

static void
sigsegv_handler(int sig)
{
    printf("\nSIGSEGV exiting...\n");
    abort();
    /*NOTREACHED*/
    assert(0);
}

static void
sigint_handler(int sig)
{
    printf("\n^C exiting...\n");
    exit(0);
    /*NOTREACHED*/
    assert(0);
}

static void /*NOTRETURN*/
usage(int status)
{
    printf("minerva library fuzzer\n"
      "\n"
      " -h\t\thelp\n"
      " -i num\t\tnumber of iterations\n"
      " -s seed\tseed for random generator\n"
      " -S\t\thandle SIGSEGV\n"
      " -r\t\tshell mode\n"
      " -T\t\tsave trace to file\n"
      " -p filename\tplay trace from file\n"
      " -M\t\tmutate mode"
      "\n");
    exit(status);
    assert(0);
}

int
main(int argc, char **argv)
{
    int ch;
    char *trace_filename = NULL, *play_filename = NULL;
    char handle_sigsegv = 0, repl = 0, mutate = 0;
    unsigned int iter = 0;
    uint32_t seed;
    minerva_vars_t *vars = NULL;
    minerva_funcs_t *funcs = NULL;
    minerva_trace_t *trace = NULL;

#ifdef __linux__
    /* XXXsn: improve randomness */
    seed = random();
#else
    seed = arc4random();
#endif

    while ((ch = getopt(argc, argv, "hi:s:SrT:p:M")) != -1) {
        switch (ch) {
        case 'h':
            usage(EXIT_SUCCESS);
            /*NOTREACHED*/
            break;
        case 's':
            seed = strtoul(optarg, NULL, 10);
            break;
        case 'i':
            iter = strtoul(optarg, NULL, 10);
            break;
        case 'M':
            mutate = 1;
        case 'S':
            handle_sigsegv = 1;
            break;
        case 'r':
            repl = 1;
            break;
        case 'T':
            trace_filename = xstrdup(optarg);
            break;
        case 'p':
            play_filename = xstrdup(optarg);
            break;
        default:
            usage(EXIT_FAILURE);
            /*NOTREACHED*/
        }
    }  

    printf("seed: %u\n", seed);
    srand(seed);

    /* signal handlers - ^C - SIGINT */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    if (handle_sigsegv == 1) {
        /* signal handlers - SIGSEGV */

        if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
            fprintf(stderr, "can't install signal handler\n");
            exit(EXIT_FAILURE);
        }
    }

    if (play_filename != NULL) {
        funcs =  minerva_funcs_new();
        minerva_funcs_init(funcs);
        trace = minerva_trace_restore(play_filename, funcs);
        minerva_trace_play(trace);
        return EXIT_SUCCESS;
    }
    
    /* MAIN LOOP */
    if (repl != 0)
        minerva_repl();
    else {
        if (mutate != 0) {
            for(;;) {
                if (minerva_loop(iter, &vars, &funcs, &trace, 0) ||
                  minerva_loop(iter/128, &vars, &funcs, &trace, 1))
                    break;
            }
        } else {
            minerva_loop(iter, &vars, &funcs, &trace, 0);
        }
        if (trace_filename != NULL)
        minerva_trace_save(trace, trace_filename);
        minerva_funcs_destroy(funcs);
        minerva_vars_destroy(vars);
        minerva_trace_destroy(trace);
    }

   
    return EXIT_SUCCESS;
}
