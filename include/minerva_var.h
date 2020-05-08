/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_VAR_H_
#define _MINERVA_VAR_H_

#include <sys/queue.h>
#include <target.h>
#include <tree.h>

typedef RB_HEAD(minerva_var_tree, minerva_var_t) minerva_var_rb_t;

typedef struct {
    minerva_var_rb_t *vars;
    unsigned int *len;
    unsigned int cur_id;
} minerva_vars_t;

typedef enum {
    F_VAR_NONE = 0,          /* none */
    F_VAR_DESTROY = (1<<0),  /* destroy argument after call */
    F_VAR_UNIQUE  = (1<<1),  /* MUST be unique argument in call */
    F_VAR_ALLOC   = (1<<2),  /* indicates allocated memory */
    F_VAR_MUTATE  = (1<<3)   /* function mutates variable */
} minerva_arg_flag_t;

typedef struct minerva_var_t {
    minerva_type_t type;        /* variable type */
    void *val;                  /* value */
    minerva_arg_flag_t  flags;  /* variable flags */
    unsigned int id;
    RB_ENTRY(minerva_var_t) entries;
} minerva_var_t;

RB_PROTOTYPE(minerva_var_tree, minerva_var_t, entries, minerva_var_rb_cmp);

void minerva_vars_print_stats(minerva_vars_t *);
minerva_vars_t *minerva_vars_new(void);
minerva_var_t *minerva_var_new(minerva_vars_t *, minerva_type_t,
  unsigned int);
void minerva_var_destroy(minerva_vars_t *, minerva_var_t *);
void minerva_vars_destroy(minerva_vars_t *);
minerva_var_t *minerva_var_get(minerva_vars_t *, minerva_type_t);
minerva_var_t *minerva_var_find(minerva_vars_t *, minerva_type_t, unsigned int);
void minerva_vars_evolution(minerva_vars_t *);

typedef char *minerva_stringify_func_t(void *);

typedef struct minerva_var_stringify_t {
    minerva_type_t type;
    minerva_stringify_func_t    *func;
} minerva_var_stringify_t;

extern const minerva_var_stringify_t minerva_var_stringify_funcs[];
char *minerva_var_stringify(minerva_type_t type, void *obj);

#endif /* ! _MINERVA_VAR_H_ */
