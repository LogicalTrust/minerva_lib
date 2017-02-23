/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef __linux__
#define _WITH_GETLINE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <assert.h>

#include <xmalloc.h>
#include <minerva_trace.h>
#include <minerva_func.h>
#include <minerva_call.h>
#include <minerva_assert.h>
#include <minerva_signal.h>
#include <minerva_repl.h>

#ifdef WITH_PROGRESSBAR
#include <progressbar.h>
#endif /* ! WITH_PROGRESSBAR */

static jmp_buf segv_env;

minerva_trace_t *
minerva_trace_new()
{
    minerva_trace_t *trace = xcalloc(1, sizeof(minerva_trace_t));
    trace->calls = xcalloc(1, sizeof(minerva_fuzz_call_t));
    TAILQ_INIT(trace->calls);
    return trace;
}

void
minerva_trace_record(minerva_trace_t *trace, minerva_var_t *new_var,
        minerva_func_t *func, minerva_var_t **args)
{
    minerva_fuzz_call_t *fuzz_call = xcalloc(1, sizeof(minerva_fuzz_call_t));
    int i;

    trace->calls_num++;
    fuzz_call->func = func;
    fuzz_call->result_id = new_var->id;
    fuzz_call->arg_ids = NULL;
    if (func->arg_num > 0) {
        fuzz_call->arg_ids = xcalloc(func->arg_num, sizeof(func->arg_num));
    }
    for (i = 0; i < func->arg_num; ++i) {
        fuzz_call->arg_ids[i] = args[i]->id;
    }
    TAILQ_INSERT_TAIL(trace->calls, fuzz_call, entries);
}

int
minerva_trace_play(minerva_trace_t *trace)
{
    minerva_vars_t *vars;
    minerva_fuzz_call_t *call;
    minerva_var_t *new_var;
    minerva_var_t **args;
#ifdef PROGRESS_BAR
    progressbar *progress;
#endif /* !PROGRESS_BAR */
    int i;
    int r = R_PLAY_NOTCRASHED;


    vars = minerva_vars_new();

#ifdef PROGRESS_BAR
    progress = progressbar_new("Replay", trace->calls_num);
#endif /* !PROGRESS_BAR */


    if (setjmp(segv_env)) {
        if (call == TAILQ_LAST(trace->calls, minerva_calls_t))
            r = R_PLAY_CRASHEDLAST;
        else
            r = R_PLAY_CRASHEDMIDDLE;
    } else {
        minerva_signal_setup(&segv_env);
        TAILQ_FOREACH(call, trace->calls, entries) {
            new_var = xcalloc(1, sizeof(minerva_var_t));
            new_var = minerva_var_new(vars, call->func->return_type,
                call->result_id);
            args = NULL;
            if (call->func->arg_num > 0) {
                args = xcalloc(call->func->arg_num, sizeof(minerva_var_t));
            }
            for (i = 0; i < call->func->arg_num; ++i) {
                args[i] = minerva_var_find(vars, call->func->args[i]->type,
                  call->arg_ids[i]);
                assert(args[i] != NULL);
            }

            minerva_call(vars, new_var, call->func, args);
#ifdef PROGRESS_BAR
            progressbar_inc(progress);
#endif /* !PROGRESS_BAR */
        }
    }

#ifdef PROGRESS_BAR
    progressbar_finish(progress);
#endif /* !PROGRESS_BAR */
    minerva_signal_revert();

    return r;
}

void
minerva_trace_save(minerva_trace_t *trace, const char *filename)
{
    FILE *f = NULL;
    minerva_fuzz_call_t *call;
    int i;

    f = fopen(filename, "w");
    if (f == NULL) {
        printf("Couldn't open output file, nothing saved.\n");
        return;
    }

    if (VERBOSE_LEVEL(VERBOSE_NOISY)) 
    	fprintf(stderr, "saving %u calls in a trace\n", trace->calls_num);

    TAILQ_FOREACH(call, trace->calls, entries) {
        fprintf(f, "%u = %s(", call->result_id, call->func->name);

        for (i = 0; i < call->func->arg_num - 1; ++i) {
            fprintf(f, "%u, ", call->arg_ids[i]);
        }
        if (call->func->arg_num > 0) {
            fprintf(f, "%u", call->arg_ids[i]);
        }
        fprintf(f, ")\n");
    }
    fclose(f);
}

minerva_trace_t *
minerva_trace_restore(const char *filename, minerva_funcs_t* funcs)
{
    FILE *f = NULL;
    char *line = NULL;
    size_t len;
    int i;
    char func_name[128], args_string[512];
    minerva_fuzz_call_t *fuzz_call = NULL;
    minerva_trace_t *trace = minerva_trace_new();

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("Couldn't open trace file, nothing imported.\n");
        return trace;
    }

    while (getline(&line, &len, f) != -1) {
        trace->calls_num++;
        fuzz_call = xcalloc(1, sizeof(minerva_fuzz_call_t));
        sscanf(line, "%u %*s %127[^(] %*[(] %511[^\n]",
                &fuzz_call->result_id, func_name, args_string);

        fuzz_call->func = minerva_func_find(funcs, func_name);

        fuzz_call->arg_ids = NULL;
        if (fuzz_call->func->arg_num > 0) {
            fuzz_call->arg_ids = xcalloc(fuzz_call->func->arg_num, 
                    sizeof(fuzz_call->func->arg_num));
        }
        for (i = 0; i < fuzz_call->func->arg_num; ++i) {
            sscanf(args_string, "%u %*[, ] %511[^\n]",
                    &fuzz_call->arg_ids[i], args_string);
        }

       minerva_assert(i == fuzz_call->func->arg_num);
       TAILQ_INSERT_TAIL(trace->calls, fuzz_call, entries);
    }

    if (VERBOSE_LEVEL(VERBOSE_NOISY))
       fprintf(stderr, "imported a trace of %u calls\n", trace->calls_num);
    free(line);
    fclose(f);

    return trace;
}

static minerva_fuzz_call_t *
minerva_fuzz_call_copy(minerva_fuzz_call_t *call, minerva_funcs_t *funcs)
{
    minerva_fuzz_call_t *copy;
    int i;

    copy = xcalloc(1, sizeof(*copy));
    copy->func = minerva_func_find(funcs, call->func->name);


    copy->arg_ids = NULL;
    if (copy->func->arg_num > 0) {
        copy->arg_ids = xcalloc(copy->func->arg_num, 
                sizeof(copy->func->arg_num));
        for (i = 0; i < copy->func->arg_num; i++)
            copy->arg_ids[i] = call->arg_ids[i];
    }
    copy->result_id = call->result_id;

    return copy;
}

minerva_trace_t *
minerva_trace_minimize_naive(minerva_trace_t *trace, minerva_funcs_t *funcs)
{
    minerva_fuzz_call_t *call;
    /* XXX - assuming all var ids to be nonnegative and less then calls_num */
    char *relevant = xcalloc(trace->calls_num, sizeof(char));
    int i;
    minerva_trace_t *min_trace = minerva_trace_new();
    minerva_fuzz_call_t *copy;

    /* This just removes fuzz calls irrelevant to the crash. */
    relevant[TAILQ_LAST(trace->calls, minerva_calls_t)->result_id] = 1;
    TAILQ_FOREACH_REVERSE(call, trace->calls, minerva_calls_t, entries) {
        if (relevant[call->result_id]) {
            for (i = 0; i < call->func->arg_num; ++i) {
                relevant[call->arg_ids[i]] = 1;
            }
            min_trace->calls_num++;
            copy = minerva_fuzz_call_copy(call, funcs);
            TAILQ_INSERT_HEAD(min_trace->calls, copy, entries);
        }
    }

    return min_trace;
}

minerva_trace_t *
minerva_trace_minimize(minerva_trace_t *trace, minerva_funcs_t *funcs,
        unsigned int rounds)
{
    minerva_trace_t *current, *min_trace;
    minerva_fuzz_call_t *call;
    int i;
    unsigned int *relevant = xcalloc(trace->calls_num, sizeof(unsigned int));
    unsigned int *removed = xcalloc(trace->calls_num, sizeof(unsigned int));
    minerva_vars_t *vars = NULL;
    minerva_var_t *new_var = NULL;
    minerva_var_t **args = NULL;

    /* TODO - replace this list with an array? */
    typedef struct elem_t {
        unsigned int id;
        LIST_ENTRY(elem_t) entries;
    } elem_t;
    typedef struct list_t list_t;
    LIST_HEAD(list_t, elem_t);
    elem_t *e, *f;
    list_t irrelevant;
    char crashed;

    LIST_INIT(&irrelevant);
    /* Marks fuzz calls potentially irrelevant to the crash and discover
     * dependecies of calls. */
    relevant[TAILQ_LAST(trace->calls, minerva_calls_t)->result_id] = 1;
    TAILQ_FOREACH_REVERSE(call, trace->calls, minerva_calls_t, entries) {
        if (relevant[call->result_id]) {
            for (i = 0; i < call->func->arg_num; ++i) {
                relevant[call->arg_ids[i]] = 1;
            }
        } else {
            e = xcalloc(1, sizeof(elem_t));
            e->id = call->result_id;
            LIST_INSERT_HEAD(&irrelevant, e, entries);
        }
    }

    minerva_signal_setup(&segv_env);

    min_trace = minerva_trace_copy(trace, funcs);
    LIST_FOREACH(e, &irrelevant, entries) {
        if (rounds == 0) {
            break;
        }
        if (rand() % 2) {
            // try and remove e->id together with dependant calls
            memset(removed, 0, sizeof(unsigned int) * trace->calls_num);
            removed[e->id] = 1;
            current = minerva_trace_new();
            vars = minerva_vars_new();
            crashed = 0;
            TAILQ_FOREACH(call, min_trace->calls, entries) {
                if (!removed[call->result_id]) {
                    args = NULL;
                    if (call->func->arg_num > 0) {
                        args = xcalloc(call->func->arg_num,
                                sizeof(minerva_var_t));
                    }
                    for (i = 0; i < call->func->arg_num; ++i) {
                        args[i] = minerva_var_find(vars,
                                call->func->args[i]->type,
                                call->arg_ids[i]);
                        // argument not found - call must have depended
                        // on a removed one
                        if (args[i] == NULL) {
                            removed[call->result_id] = 1;
                            goto next_call;
                        }
                    }
                    new_var = xcalloc(1, sizeof(minerva_var_t));
                    new_var =
                        minerva_var_new(vars, call->func->return_type,
                                call->result_id);

                    if (setjmp(segv_env)) {
                        segv = 0;
                        crashed = 1;
                        if (call != TAILQ_LAST(min_trace->calls,
                                    minerva_calls_t)) {
                            minerva_repl_error("crashed in the middle!");
                        }
                        min_trace = minerva_trace_copy(current, funcs);
                        // these calls are removed just now
                        LIST_FOREACH(f, &irrelevant, entries) {
                            if (removed[f->id]) {
                                LIST_REMOVE(f, entries);
                            }
                        }
                        break;
                    } else {
                        minerva_trace_record(current, new_var,
                                call->func, args);
                        minerva_call(vars, new_var, call->func, args);
                    }
                }
next_call:;
            }
            if (!crashed) {
                // call proved to be influential in reproducing the crash
                LIST_REMOVE(e, entries);
            }
            minerva_vars_destroy(vars);
            minerva_trace_destroy(current);
            rounds--;
        }
    }

    minerva_signal_revert();

    while(!LIST_EMPTY(&irrelevant)) {
        e = LIST_FIRST(&irrelevant);
        LIST_REMOVE(e, entries);
        xfree(e);
    }
    xfree(removed);
    xfree(relevant);

    return min_trace;
}

static void
minerva_trace_call_destroy(minerva_calls_t *head, minerva_fuzz_call_t *call)
{
    free(call->arg_ids);
    TAILQ_REMOVE(head, call, entries);
    free(call);
}

void
minerva_trace_destroy(minerva_trace_t *trace)
{
    while (!TAILQ_EMPTY(trace->calls))
        minerva_trace_call_destroy(trace->calls, TAILQ_FIRST(trace->calls));
    free(trace->calls);
    free(trace);

}

minerva_trace_t *
minerva_trace_copy(minerva_trace_t *original, minerva_funcs_t *funcs)
{
    minerva_trace_t *copy = minerva_trace_new();
    minerva_fuzz_call_t *call, *cc;

    TAILQ_FOREACH(call, original->calls, entries) {
        cc = minerva_fuzz_call_copy(call, funcs);
        TAILQ_INSERT_TAIL(copy->calls, cc, entries);
    }
    copy->calls_num = original->calls_num;

    return copy;
}
