/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_REPL_H_
#define _MINERVA_REPL_H_

#include <minerva_parser.h>
#include <minerva_dict.h>

typedef enum {
    VERBOSE_NORMAL = 0,
    VERBOSE_NOISY  = 1,
    VERBOSE_DEBUG  = 2
} minerva_verbose_t;

#define VERBOSE_LEVEL(x) (verbose >= (x))

extern int verbose;

typedef minerva_dict_var_t *minerva_repl_func_t(int argc,
          minerva_dict_var_t **args);

typedef struct {
    const char *fname;
    minerva_repl_func_t *faddr;
    int num_arg;
    const char *desc;
} minerva_repl_funcs_t;

void minerva_repl_eval_line(dict_t *, minerva_line_t *);
void minerva_repl_error(const char *);
void minerva_repl_line_destroy(minerva_line_t *);
void minerva_repl(void);

#endif
