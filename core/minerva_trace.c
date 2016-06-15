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
#include <signal.h>
#include <setjmp.h>
#include <assert.h>

#include <xmalloc.h>
#include <minerva_trace.h>
#include <minerva_func.h>
#include <minerva_call.h>
#include <minerva_assert.h>

#include <progressbar.h>

static jmp_buf segv_env;
static sig_atomic_t segv = 0;

static void
sigsegv_minerva_trace(int sig)
{
    if (segv == 1) {
        fprintf(stderr, "panic: segv in segv...");
        exit(EXIT_FAILURE);
    }
    segv = 1;
    longjmp(segv_env, 1);
}

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
        fuzz_call->arg_ids = xcalloc(func->arg_num, sizeof(unsigned));
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
    progressbar *progress;
    struct sigaction sa = {}, oldsa;
    int i;
    int r = R_PLAY_NOTCRASHED;

    sa.sa_handler = sigsegv_minerva_trace;
    sa.sa_flags = SA_NODEFER | SA_RESETHAND;
    /* signal handler - sigsegv */
    if (sigaction(SIGSEGV, &sa, &oldsa) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

    vars = minerva_vars_new();

    progress = progressbar_new("Replay", trace->calls_num);

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

        if (setjmp(segv_env)) {
            segv = 0;
            if (call == TAILQ_LAST(trace->calls, minerva_calls_t))
                r = R_PLAY_CRASHEDLAST;
            else
                r = R_PLAY_CRASHEDMIDDLE;

            break;
        } else
            minerva_call(vars, new_var, call->func, args);
        progressbar_inc(progress);
    }

    progressbar_finish(progress);

    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    if (sigaction(SIGSEGV, &sa, &oldsa) != 0) {
        fprintf(stderr, "can't install signal handler\n");
        exit(EXIT_FAILURE);
    }

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
    
#ifdef MINERVA_DEBUG
    fprintf(stderr, "saving %u calls in a trace\n", trace->calls_num);
#endif

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
            fuzz_call->arg_ids = 
                xcalloc(fuzz_call->func->arg_num, sizeof(unsigned));
        }
        for (i = 0; i < fuzz_call->func->arg_num; ++i) {
            sscanf(args_string, "%u %*[, ] %511[^\n]", 
                    &fuzz_call->arg_ids[i], args_string);
        }

#ifdef MINERVA_DEBUG
       minerva_assert(i == fuzz_call->func->arg_num); 
#endif
       TAILQ_INSERT_TAIL(trace->calls, fuzz_call, entries);
    }

#ifdef MINERVA_DEBUG
       fprintf(stderr, "imported a trace of %u calls\n", trace->calls_num);
#endif
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
        copy->arg_ids = xcalloc(copy->func->arg_num, sizeof(unsigned));
        for (i = 0; i < copy->func->arg_num; i++)
            copy->arg_ids[i] = call->arg_ids[i];
    }
    copy->result_id = call->result_id;

    return copy;
}

minerva_trace_t *
minerva_trace_minimize(minerva_trace_t *trace, minerva_funcs_t *funcs)
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
