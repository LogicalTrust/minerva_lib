/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include <minerva_signal.h>

sig_atomic_t exit_loop = 0;
sig_atomic_t segv = 0;
static jmp_buf *sig_env;
static struct sigaction old_sigsegv;
static struct sigaction old_sigabrt;
static struct sigaction old_sigint;

static void
sigint_minerva_loop(int sig)
{
    if (exit_loop != 0) {
        fprintf(stderr, "panic: ^C twice?...");
        exit(EXIT_FAILURE);
    }
    exit_loop = 1;
    longjmp(*sig_env, 1);
}

static void
sigsegv_minerva_loop(int sig)
{
    if (segv == 1) {
        fprintf(stderr, "panic: segv in segv...");
        exit(EXIT_FAILURE);
    }
    segv = 1;
    longjmp(*sig_env, 1);
}

/* setup handlers for SIGSEGV and SIGINT during fuzzing... */
void
minerva_signal_setup(jmp_buf *env)
{
    struct sigaction sa = {};

    segv = 0;
    exit_loop = 0;
    sig_env = env;

    sa.sa_handler = sigsegv_minerva_loop;
    sa.sa_flags = SA_NODEFER;
    /* signal handler - sigsegv */
    if (sigaction(SIGSEGV, &sa, &old_sigsegv) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigsegv_minerva_loop;
    sa.sa_flags = SA_NODEFER | SA_RESETHAND;
    /* signal handler - sigabrt (for ASAN) */
    if (sigaction(SIGABRT, &sa, &old_sigabrt) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigint_minerva_loop;
    sa.sa_flags = 0;
    /* signal handler - ^c - sigint */
    if (sigaction(SIGINT, &sa, &old_sigint) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }
}

/* XXX: cheesy name */
void
minerva_signal_revert(void)
{
    /* signal handler - sigsegv */
    if (sigaction(SIGSEGV, &old_sigsegv, NULL) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    /* signal handler - sigabrt */
    if (sigaction(SIGABRT, &old_sigabrt, NULL) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    /* signal handler - ^c - sigint */
    if (sigaction(SIGINT, &old_sigint, NULL) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    segv = 0;
    exit_loop = 0;
}
