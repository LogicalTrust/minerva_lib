/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_LOOP_H_
#define _MINERVA_LOOP_H_

#include <sys/types.h>

#include <minerva_var.h>
#include <minerva_func.h>
#include <minerva_trace.h>

int minerva_loop(unsigned int, minerva_vars_t **, minerva_funcs_t **,
  minerva_trace_t **, int);

#endif
