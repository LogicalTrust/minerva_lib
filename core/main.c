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
#include <random.h>
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
    printf("Minerva library fuzzer\n"
      "\n"
      " -h\t\thelp\n"
      " -i num\t\tnumber of iterations\n"
      " -s seed\tseed for random generator\n"
      " -S\t\thandle SIGSEGV\n"
      " -r\t\tshell mode\n"
      " -T filename\tsave trace to file\n"
      " -p filename\tplay trace from file\n"
      " -M\t\tmutate mode\n"
      " -D filename\tsave stringify dump to file"
      "\n");

    exit(status);
}

int
main(int argc, char **argv)
{
    int ch;
    char *trace_filename = NULL, *play_filename = NULL,
      *stringify_filename = NULL;
    char handle_sigsegv = 0, repl = 0, mutate = 0, handle_sigint = 1;
    unsigned int iter = 0;
    uint32_t seed;
    minerva_vars_t *vars = NULL;
    minerva_funcs_t *funcs = NULL;
    minerva_trace_t *trace = NULL;

#ifdef __linux__
    getentropy(&seed, sizeof(seed));
#else
    seed = arc4random();
#endif

    while ((ch = getopt(argc, argv, "hi:s:SrT:p:MCvD:")) != -1) {
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
        case 'C':
            handle_sigint = 0;
            break;
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
        case 'v':
            verbose++;
            break;
        case 'D':
            stringify_filename = xstrdup(optarg);
            break;
        default:
            usage(EXIT_FAILURE);
            /*NOTREACHED*/
        }
    }  

    if (VERBOSE_LEVEL(VERBOSE_NOISY))
        fprintf(stderr, "seed: %u\n", seed);

    __srand(seed);

    if (handle_sigint == 1) {
        /* signal handlers - ^C - SIGINT */
        if (signal(SIGINT, sigint_handler) == SIG_ERR) {
            fprintf(stderr, "can't install signal handler\n");
            exit(EXIT_FAILURE);
        }
    }

    if (handle_sigsegv == 1) {
        /* signal handlers - SIGSEGV */

        if (signal(SIGSEGV, sigsegv_handler) == SIG_ERR) {
            fprintf(stderr, "can't install signal handler\n");
            exit(EXIT_FAILURE);
        }
    }

    /* REPL */
    if (repl != 0) {
        minerva_repl();
        return EXIT_SUCCESS;
    }

    if (play_filename != NULL) {
        funcs = minerva_funcs_new();
        vars = minerva_vars_new();
        minerva_funcs_init(funcs);
        trace = minerva_trace_restore(play_filename, funcs);
        minerva_trace_play(trace, vars);
        goto out;
    }
    
    /* MAIN LOOP */
    if (mutate != 0) {
        for(;;) {
            if (minerva_loop(iter, &vars, &funcs, &trace, 0) ||
              minerva_loop(iter/128, &vars, &funcs, &trace, 1))
                break;
        }
    } else {
        minerva_loop(iter, &vars, &funcs, &trace, 0);
    }
   
out:
    if (trace_filename != NULL)
        minerva_trace_save(trace, trace_filename);
    if (stringify_filename != NULL)
        minerva_vars_stringify(vars, stringify_filename);
    minerva_funcs_destroy(funcs);
    minerva_vars_destroy(vars);
    minerva_trace_destroy(trace);

    return EXIT_SUCCESS;
}
