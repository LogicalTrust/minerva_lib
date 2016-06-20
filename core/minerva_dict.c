/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>

#include "dict.h"
#include <minerva_dict.h>
#include <minerva_func.h>
#include <minerva_var.h>
#include <minerva_trace.h>

dict_t *
minerva_dict_create(void)
{
    return dict_create();
}

int
minerva_dict_set(dict_t *dict, const char *key, minerva_dict_var_t *var)
{
    return dict_insert(dict, key, var);
}

minerva_dict_var_t *
minerva_dict_get(dict_t *dict, const char *key)
{
    return dict_get(dict, key);
}

void
minerva_dict_destroy(dict_t *dict)
{
    return dict_destroy(dict);
}

void
minerva_dict_var_destroy(minerva_dict_var_t *var)
{
    minerva_dict_var_result_t *result;

    if (var == NULL)
        return;

    switch (var->type) {
        case MINERVA_RESULT_T:
            result = var->val.var;
            MINERVA_REFS_DEC(result);
            if (MINERVA_REFS(result) != 0)
                break;
            minerva_vars_destroy(result->vars);
            minerva_funcs_destroy(result->funcs);
            minerva_trace_destroy(result->trace);
            free(result);
            break;
        case MINERVA_STRING_T:
            free(var->val.str);
            break;
        default:
            /* do nothing */
            break;
    }

    free(var);
}
