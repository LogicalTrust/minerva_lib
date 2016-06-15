/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_DICT_H_
#define _MINERVA_DICT_H_
#include <minerva_loop.h>
#include <minerva_trace.h>
#include <minerva_refs.h>

typedef enum {
    MINERVA_RESULT_T,
    MINERVA_TRACE_T,
    MINERVA_VAR_T,
    MINERVA_DATABASE_T,
    MINERVA_STRING_T,
    MINERVA_NUMBER_T
} minerva_dict_var_type_t;

typedef struct {
    minerva_dict_var_type_t type;
    union {
        int num;
        char *str;
        void *var;
    } val;
} minerva_dict_var_t;

typedef struct {
    minerva_refs_t refs;
    minerva_vars_t *vars;
    minerva_funcs_t *funcs;
    minerva_trace_t *trace;
} minerva_dict_var_result_t;

typedef struct dict_t dict_t;

dict_t *minerva_dict_create(void); 
int minerva_dict_set(dict_t *, const char *, minerva_dict_var_t *);
minerva_dict_var_t *minerva_dict_get(dict_t *, const char *);
void minerva_dict_destroy(dict_t *);
void minerva_dict_var_destroy(minerva_dict_var_t *);
#endif
