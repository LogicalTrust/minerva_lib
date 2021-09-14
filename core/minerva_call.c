/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>

#include <minerva_assert.h>
#include <minerva_call.h>
#include <minerva_func.h>
#include <minerva_var.h>
#include <minerva_trace.h>
#include <minerva_repl.h>
#include <target.h>
#include <random.h>
#include <xmalloc.h>

void
minerva_call(minerva_vars_t *vars, minerva_var_t *new_var,
  minerva_func_t *call_func, minerva_var_t **call_vars)
{
    int i;

    minerva_assert(new_var != NULL);
    minerva_assert(call_func != NULL);
    minerva_assert((call_func->arg_num > 0 && call_vars != NULL) ||
      call_func->arg_num == 0);

    if (VERBOSE_LEVEL(VERBOSE_NOISY)) {
        fprintf(stderr, "calling %s", call_func->name);
        fprintf(stderr, "(");
        for (i = 0; i < call_func->arg_num ; i++) {
            fprintf(stderr, "%d", call_vars[i]->id);
            if (i+1 != call_func->arg_num)
                fprintf(stderr, ", ");
        }
        fprintf(stderr, ") ");
        fflush(stderr);
    }

    /* call wrapper */
    call_func->func(new_var, call_vars);

    /*
     * XXXSHM:
     * It's ambigious if vars with F_VAR_DESTROY flag should be removed in
     * general or only in case when call failed. For safety we're doing it
     * in all cases.
     */

    for (i = 0; i < call_func->arg_num ; i++) {
        if ((call_func->args[i]->flags & F_VAR_DESTROY) != 0) {
            if (VERBOSE_LEVEL(VERBOSE_NOISY)) {
                fprintf(stderr, "!!! destroy: %d ", call_vars[i]->id);
            }
            minerva_var_destroy(vars, call_vars[i]);
        }
    }

    return;
}

int
minerva_random_call(minerva_vars_t *vars, minerva_funcs_t *funcs,
  minerva_trace_t *trace, int mutate)
{
    minerva_func_t *call_func = minerva_func_get(funcs, vars, mutate);
    minerva_var_t **call_vars = NULL;
    minerva_var_t *new_var = NULL;
    int result;
    int i, j, unique;

    minerva_assert(call_func != NULL);

    if (call_func->arg_num > 0)
        call_vars = xcalloc(call_func->arg_num, sizeof(*call_vars));

    /* generating parameters looks as follows:
     * 1) first get variables for UNIQUE args (and ensure that they're
     *    unique)
     * 2) get variables for other args (and ensure that they don't collide
     *    with UNIQUE ones)
     */

    for (i = 0; i < call_func->arg_num; i++) {
        if ((call_func->args[i]->flags & F_VAR_UNIQUE) != 0) {
            while (1) {
                call_vars[i] = minerva_var_get(vars, call_func->args[i]->type);
                for (j = 0, unique = 1; j < i; j++) {
                    if (call_vars[i] == call_vars[j]) {
                        unique = 0;
                        break;
                    }
                }
                if (unique != 0)
                    break;
            }
        } 
    }

    for (i = 0; i < call_func->arg_num; i++) {
        if ((call_func->args[i]->flags & F_VAR_UNIQUE) == 0) {
            while (1) {
                call_vars[i] = minerva_var_get(vars, call_func->args[i]->type);
                for (j = 0, unique = 1; j < i; j++) {
                    if (((call_func->args[j]->flags & F_VAR_UNIQUE) != 0) &&
                       call_vars[i] == call_vars[j]) {
                        unique = 0;
                        continue;
                    }
                }
                if (unique != 0)
                    break;
            }
        } 
    }
    
    for (i = 0; i < call_func->arg_num; i++) 
        minerva_assert(call_vars[i] != NULL);

    new_var = minerva_var_new(vars, call_func->return_type, 0);
    minerva_trace_record(trace, new_var, call_func, call_vars);
    minerva_call(vars, new_var, call_func, call_vars);

    /* 
     * check if called function returned success, if no check function is
     * provided, then functions cannot fail. In this case check function is
     * indicated by NULL
     */

    if (call_func->check == NULL || (call_func->check)(new_var->val) == 1) {
        call_func->success++;
        if (VERBOSE_LEVEL(VERBOSE_NOISY)) {
            fprintf(stderr, "success");
        }
        result = 1;
    } else {
        call_func->failure++;
        if (VERBOSE_LEVEL(VERBOSE_NOISY)) {
            fprintf(stderr, "failed");
        }
        result = 0;
        minerva_var_destroy(vars, new_var);
    } 

    if (VERBOSE_LEVEL(VERBOSE_NOISY)) {
        fprintf(stderr, "\n");
    }

    xfree(call_vars);

    return result;
}
