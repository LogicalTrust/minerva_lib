/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_TRACE_H_
#define _MINERVA_TRACE_H_

#include <sys/queue.h>
#include <minerva_var.h>
#include <minerva_func.h>

typedef struct minerva_fuzz_call_t {
    unsigned int result_id;
    minerva_func_t *func;
    unsigned int *arg_ids;
    TAILQ_ENTRY(minerva_fuzz_call_t) entries;
} minerva_fuzz_call_t;
typedef struct minerva_calls_t minerva_calls_t;
TAILQ_HEAD(minerva_calls_t, minerva_fuzz_call_t);

typedef struct minerva_trace_t {
    minerva_calls_t *calls;
    unsigned int calls_num;
} minerva_trace_t;

typedef enum {
    R_PLAY_NOTCRASHED = 0,
    R_PLAY_CRASHEDLAST = 1,
    R_PLAY_CRASHEDMIDDLE = 2
} trace_play_result_t;

minerva_trace_t *minerva_trace_new();
void minerva_trace_record(minerva_trace_t *, minerva_var_t *, minerva_func_t *, 
        minerva_var_t **);
int minerva_trace_play(minerva_trace_t *, minerva_vars_t *);
void minerva_trace_save(minerva_trace_t*, const char*);
minerva_trace_t *minerva_trace_restore(const char*, minerva_funcs_t *);
minerva_trace_t *minerva_trace_minimize(minerva_trace_t*, minerva_funcs_t*,
        unsigned int);
void minerva_trace_destroy(minerva_trace_t *);
minerva_trace_t *minerva_trace_copy(minerva_trace_t *, minerva_funcs_t *);
#endif /* ! _MINERVA_TRACE_H_ */
