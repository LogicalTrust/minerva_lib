/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_FUNC_H_
#define _MINERVA_FUNC_H_

#include <stdint.h>
#include <sys/types.h>
#include <sys/queue.h>
#include <minerva_var.h>

typedef enum {
    F_FUN_NONE   = 0,      /* none */
    F_FUN_MUTATE = (1<<0)  /* function mutates variables */
} minerva_func_flags_t;

typedef uint8_t minerva_arg_flags_t;

typedef int minerva_check_t(const void *);

typedef void *minerva_function_t(minerva_var_t *new_var,
  minerva_var_t **vars);

typedef struct {
    minerva_type_t type;
    minerva_arg_flags_t flags; 
} minerva_arg_t;

typedef struct minerva_func_t {
    minerva_type_t return_type;
    uint32_t success;
    uint32_t failure;
    minerva_function_t *func;
    const char *name;
    minerva_func_flags_t flags;
    minerva_check_t *check;         /* checking function */
    LIST_ENTRY(minerva_func_t) entries;
    uint8_t arg_num;
    unsigned int id;
    minerva_arg_t **args;
} minerva_func_t;
typedef struct minerva_funcs_t minerva_funcs_t;
LIST_HEAD(minerva_funcs_t, minerva_func_t);

void minerva_funcs_print_stats(minerva_funcs_t *funcs);

minerva_funcs_t *minerva_funcs_new(void);
void minerva_funcs_init(minerva_funcs_t *);
void minerva_funcs_add(minerva_funcs_t *funcs, const minerva_type_t return_type,
  void *fun, const char *name, const minerva_func_flags_t flags,
  minerva_check_t *check, const int arg_num, ...);
minerva_func_t *minerva_func_get(minerva_funcs_t *, minerva_vars_t *, int);
minerva_func_t *minerva_func_find(minerva_funcs_t *, const char *);
void minerva_funcs_destroy(minerva_funcs_t *);
#endif /* ! _MINERVA_FUNC_H_ */
