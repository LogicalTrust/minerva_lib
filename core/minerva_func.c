/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/queue.h>

#include <minerva_assert.h>
#include <minerva_var.h>
#include <minerva_func.h>
#include <xmalloc.h>
#include <target.h>

void
minerva_funcs_print_stats(minerva_funcs_t *funcs)
{
    uint32_t total;
    minerva_func_t *func;
    float success_rate, failure_rate;

    fprintf(stderr, "\n#### FUNC STATS #####\n");
    LIST_FOREACH(func, funcs, entries) {
        total = func->success + func->failure;
        if ( total != 0 ) {
            success_rate = (func->success / (float)total) * 100;
            failure_rate = (func->failure / (float)total) * 100;
            fprintf(stderr, "%25s %3.1f %5d %3.1f %5d %10d\n", func->name,
              success_rate, func->success, failure_rate, func->failure,
              total );
        } else {
            fprintf(stderr, "%25s %3.1f %3.1f %10d\n", func->name, 0.0f, 0.0f,
              0);
        }
    }
}

minerva_funcs_t *
minerva_funcs_new(void)
{
    minerva_funcs_t *head;
    head = xcalloc(1, sizeof(*head));
    LIST_INIT(head);
    return head;
}

void
minerva_funcs_add(minerva_funcs_t *funcs,
  const minerva_type_t return_type, void *func, const char *name,
  const minerva_func_flags_t flags, minerva_check_t *check,
  const int arg_num, ...)
{
    minerva_func_t *new_func = xcalloc(1, sizeof(*new_func));
    va_list ap;
    int i;

    if (arg_num > 0)
        new_func->args = xcalloc(arg_num, sizeof(*(new_func->args)));

    new_func->return_type = return_type;
    new_func->func = func;
    new_func->flags = flags;
    new_func->check = check;
    new_func->arg_num = arg_num;
    new_func->name = name;

    va_start(ap, arg_num);
    for (i = 0; i < arg_num; i++) {
        new_func->args[i] = va_arg(ap, minerva_arg_t *);
        if ((new_func->args[i]->flags & F_VAR_MUTATE) != 0)
            new_func->flags |= F_FUN_MUTATE;
    }
    va_end(ap);

    LIST_INSERT_HEAD(funcs, new_func, entries);

    return;
}

/* checks if we've got enough variables collected in order to execute the
 * funtion
 */
static int
minerva_func_qualify(minerva_func_t *func, minerva_vars_t *vars, int mutate)
{
    unsigned int var[__minerva_types_no];
    int i, arg_type;

    if ((mutate != 0) != ((func->flags & F_FUN_MUTATE) != 0))
        return 0;

    for (i = 0; i < __minerva_types_no; i++)
        var[i] = vars->len[i];

    for (i = 0; i < func->arg_num; i++) {
        arg_type = func->args[i]->type;

        if (var[arg_type] == 0)
            return 0;
       
        /* We can't reuse variable with F_VAR_UNIQUE flag */
        if ((func->args[i]->flags & F_VAR_UNIQUE) != 0)
            var[arg_type]--;
    }

    return 1;
}

minerva_func_t *
minerva_func_get(minerva_funcs_t *funcs, minerva_vars_t *vars, int mutate)
{
    static minerva_func_t * func_arr[MINERVA_FUNC_NUM];
    int n;
    minerva_func_t *func, *ret = NULL;

    n = 0;
    LIST_FOREACH(func, funcs, entries) {
        if (minerva_func_qualify(func, vars, mutate) != 0) {
            func_arr[n++] = func;
        }
    }
    ret = func_arr[rand() % n];
#ifdef MINERVA_DEBUG
    minerva_assert(ret != NULL);
#endif

    return ret;
}

minerva_func_t *
minerva_func_find(minerva_funcs_t *funcs, const char *name)
{
    minerva_func_t *ret = NULL;

    LIST_FOREACH(ret, funcs, entries) {
        if (strcmp(ret->name, name) == 0) {
            break;
        }
    }

#ifdef MINERVA_DEBUG
    minerva_assert(ret != NULL);
#endif

    return ret;
}

static void
minerva_func_destroy(minerva_func_t *func)
{
    /* args are allocated at once by the compiler */
    if (func->arg_num > 0)
        free(func->args[0]);
    free(func->args);
    LIST_REMOVE(func, entries);
    free(func);
}

void
minerva_funcs_destroy(minerva_funcs_t *funcs)
{
    while(!LIST_EMPTY(funcs))
        minerva_func_destroy(LIST_FIRST(funcs));

    free(funcs);
}
