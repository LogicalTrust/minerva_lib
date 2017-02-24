/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <setjmp.h>

#include <minerva_colors.h>
#include <minerva_var.h>
#include <minerva_call.h>
#include <minerva_func.h>
#include <minerva_parser.h>
#include <minerva_repl.h>
#include <minerva_signal.h>
#include <xmalloc.h>

#ifdef WITH_PROGRESSBAR
#include <progressbar.h>
#endif /* !WITH_PROGRESSBAR */

static jmp_buf sig_env;

int
minerva_loop(unsigned int iter, minerva_vars_t **vars,
  minerva_funcs_t **funcs, minerva_trace_t **trace, int mutate)
{
    int crash = 0, forever = 0;
    unsigned int i;
#ifdef WITH_PROGRESSBAR
    progressbar *progress;
#endif /* !WITH_PROGRESSBAR */

    if (iter == 0)
        forever = 1;

#ifdef WITH_PROGRESSBAR
    if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY))
        progress = progressbar_new(mutate == 0 ? "Fuzzing" : "Mutating",
          iter);
#endif /* !WITH_PROGRESSBAR */

    if (*vars == NULL)
        *vars = minerva_vars_new();

    if (*funcs == NULL) {
        *funcs = minerva_funcs_new();
        minerva_funcs_init(*funcs);
    }

    if (*trace == NULL)
        *trace = minerva_trace_new();

    if (setjmp(sig_env)) {
        if (segv != 0)
            crash = 1;
    } else {
        minerva_signal_setup(&sig_env);
        exit_loop = 0;
        for (i = 0 ; forever || i < iter; i++) {
            if (exit_loop)
                break;
            minerva_random_call(*vars, *funcs, *trace, mutate);
#ifdef WITH_PROGRESSBAR
            if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY)) {
                if (iter/100 == 0)
                    progressbar_inc(progress);
                else if (i % (iter/100) == 0) {
                    progressbar_update(progress, i);
                }
            }
#endif /* !WITH_PROGRESSBAR */
        }
    }

#ifdef WITH_PROGRESSBAR
    if (forever == 0 && !VERBOSE_LEVEL(VERBOSE_NOISY)) {
        progressbar_update(progress, iter);
        progressbar_finish(progress);
    }
#endif /* !WITH_PROGRESSBAR */

    if (crash) {
        printf(ANSI_COLOR_GREEN "\\o/ found crash *yay* \\o/"
          ANSI_COLOR_RESET "\n");
    }

    minerva_signal_revert();

    return crash;
}
