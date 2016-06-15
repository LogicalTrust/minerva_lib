/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#include <minerva_colors.h>
#include <minerva_var.h>
#include <minerva_call.h>
#include <minerva_func.h>
#include <minerva_parser.h>
#include <minerva_repl.h>
#include <xmalloc.h>

#include <progressbar.h>

static sig_atomic_t exit_loop = 0;
static sig_atomic_t segv = 0;
static jmp_buf segv_env;

void
sigint_minerva_loop(int sig)
{
    if (exit_loop != 0) {
        fprintf(stderr, "panic: ^C twice?...");
        exit(EXIT_FAILURE);
    }
    exit_loop = 1;
    longjmp(segv_env, 1);
}

void
sigsegv_minerva_loop(int sig)
{
    if (segv == 1) {
        fprintf(stderr, "panic: segv in segv...");
        exit(EXIT_FAILURE);
    }
    segv = 1;
    longjmp(segv_env, 1);
}



int
minerva_loop(unsigned int iter, minerva_vars_t **vars,
  minerva_funcs_t **funcs, minerva_trace_t **trace, int mutate)
{
    int crash = 0, forever = 0;
    unsigned int i;
    progressbar *progress;
    struct sigaction sa = {};

    if (iter == 0)
        forever = 1;

    segv = 0;
    sa.sa_handler = sigsegv_minerva_loop;
    sa.sa_flags = SA_NODEFER | SA_RESETHAND;
    /* signal handler - sigsegv */
    if (sigaction(SIGSEGV, &sa, NULL) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigint_minerva_loop;
    sa.sa_flags = 0;
    /* signal handler - ^c - sigint */
    if (sigaction(SIGINT, &sa, NULL) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY))
        progress = progressbar_new(mutate == 0 ? "Fuzzing" : "Mutating",
          iter);

    if (*vars == NULL)
        *vars = minerva_vars_new();

    if (*funcs == NULL) {
        *funcs = minerva_funcs_new();
        minerva_funcs_init(*funcs);
    }

    if (*trace == NULL)
        *trace = minerva_trace_new();

    if (setjmp(segv_env)) {
        if (segv != 0)
            crash = 1;
    } else {
        exit_loop = 0;
        for (i = 0 ; forever || i < iter; i++) {
            if (exit_loop)
                break;
            minerva_random_call(*vars, *funcs, *trace, mutate);
            if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY)) {
                if (iter/100 == 0)
                    progressbar_inc(progress);
                else if (i % (iter/100) == 0) {
                    progressbar_update(progress, i);
                }
            }
        }
    }

    if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY)) {
        progressbar_update(progress, iter);
        progressbar_finish(progress);
    }

    if (crash) {
        printf(ANSI_COLOR_GREEN "\\o/ found crash *yay* \\o/"
          ANSI_COLOR_RESET "\n");
    }

    segv = 0;

    return crash;
}
