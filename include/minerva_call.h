/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_CALL_H_
#define _MINERVA_CALL_H_

#include <minerva_var.h>
#include <minerva_func.h>
#include <minerva_trace.h>

void sigsegv_minerva_call(int sig);
void minerva_call(minerva_vars_t *, minerva_var_t *, minerva_func_t *,
  minerva_var_t **);
int minerva_random_call(minerva_vars_t *, minerva_funcs_t *,
  minerva_trace_t *, int);

#endif /* ! _MINERVA_CALL_H_ */
