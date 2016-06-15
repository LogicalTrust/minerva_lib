/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#ifndef _MINERVA_PARSER_H_
#define _MINERVA_PARSER_H_

typedef enum {
    NUMBER_VAL,
    STRING_VAL,
    VAR_VAL
} minerva_repl_arg_type_t;

typedef struct {
    minerva_repl_arg_type_t type;
    union {
        char *str;
        int num;
        char *var;
    } val;
} minerva_repl_arg_t;

typedef struct {
    char *fname;
    int num_arg;
    minerva_repl_arg_t **args;
} minerva_call_t;

/* lame */
typedef struct {
    minerva_repl_arg_t   *var;
    minerva_call_t  *call;
} minerva_line_t;

minerva_line_t *minerva_repl_parse(const char *);
#endif
