/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <assert.h>
#include <target.h>
#include <tree.h>
#include <minerva_assert.h>
#include <minerva_var.h>
#include <minerva_func.h>
#include <random.h>
#include <xmalloc.h>

static int
minerva_var_rb_cmp(minerva_var_t *a, minerva_var_t *b)
{
    if (a->id == b->id)
        return 0;
    else if (a->id > b->id)
        return 1;
    else
        return -1;
}

RB_GENERATE(minerva_var_tree, minerva_var_t, entries,
  minerva_var_rb_cmp);

void
minerva_vars_print_stats(minerva_vars_t *vars)
{
    minerva_var_t *var = NULL;
    int i;
    int n;

    fprintf(stderr, "\n#### VAR STATS #####\n");
    for (i = 0; i < __minerva_types_no; i++) {
        n = 0;
        RB_FOREACH(var, minerva_var_tree, &(vars->vars[i])) {
            n++;
        }
        var = NULL;
        fprintf(stderr, "var type: %16s\telements in list: %4d\n", minerva_type_name[i], n);
    }

    fprintf(stderr, "\n");
}

minerva_vars_t *
minerva_vars_new(void)
{
    int i;
    minerva_vars_t *vars = xcalloc(1, sizeof(*vars));
    vars->vars = xcalloc(__minerva_types_no, sizeof(*(vars->vars)));
    vars->len = xcalloc(__minerva_types_no, sizeof(*(vars->len)));

    for (i = 0; i < __minerva_types_no; i++) {
        RB_INIT(&(vars->vars[i]));
    }

    return vars;
}

minerva_var_t *
minerva_var_new(minerva_vars_t *vars, minerva_type_t type, unsigned int id)
{
    minerva_var_t *new_var = xcalloc(1, sizeof(*new_var));

    new_var->type = type;

    vars->len[type]++;

    /* used in replays: for now we don't check if another id is already
     * registered */
    if (id == 0)
        new_var->id = vars->cur_id++;
    else
        new_var->id = id;

    RB_INSERT(minerva_var_tree, &(vars->vars[type]), new_var);

    return new_var;
}

void
minerva_var_destroy(minerva_vars_t *vars, minerva_var_t *var)
{
    if (var == NULL)
        return;

    if ((var->flags & F_VAR_ALLOC) != 0)
        xfree(var->val);

    vars->len[var->type]--;

    RB_REMOVE(minerva_var_tree, &(vars->vars[var->type]), var);
    xfree(var);
}

void
minerva_vars_stringify(minerva_vars_t *vars, const char *stringify_filename)
{
    FILE *f;
    char *s;
    int i;
    minerva_var_t *var;

    f = fopen(stringify_filename, "w");

    if (f == NULL) {
        fprintf(stderr,
          "Couldn't open stringify output file %s, nothing saved.\n",
          stringify_filename);
        return;
    }

    for (i = 0; i < __minerva_types_no; i++) {
        RB_FOREACH(var, minerva_var_tree, &(vars->vars[i])) {
            minerva_assert(var->type == i);
            s = minerva_var_stringify(var->type, var->val);
            if (s != NULL)
                fprintf(f, "%u: %s\n", var->id, s);
            xfree(s);
        }
    }

    fclose(f);

    return;
}

void
minerva_vars_destroy(minerva_vars_t *vars)
{
    int i;

    if (vars == NULL)
        return;

    for (i = 0; i < __minerva_types_no; i++) {
        while(!RB_EMPTY(&(vars->vars[i]))) {
            minerva_var_destroy(vars, RB_ROOT(&(vars->vars[i])));
        }
    }

    for (i = 0; i < __minerva_types_no; i++) {
        minerva_assert(vars->len[i] == 0);
    }

    free(vars->len);
    free(vars->vars);
    free(vars);
}


minerva_var_t *
minerva_var_get(minerva_vars_t *vars, minerva_type_t type)
{
    minerva_var_t *var, *ret = NULL;
    minerva_var_t elm;

    /* XXXSHM: make it uniform rand again... for now it's good enough */
    var = RB_MAX(minerva_var_tree, &(vars->vars[type]));
    elm.id = __rand() % (var->id+1);
    ret = RB_NFIND(minerva_var_tree, &(vars->vars[type]), &elm);
    assert(ret != NULL);

    return ret;
}

minerva_var_t *
minerva_var_find(minerva_vars_t *vars, minerva_type_t type, unsigned id)
{
    minerva_var_t elm;
    elm.id = id;

    return RB_FIND(minerva_var_tree, &(vars->vars[type]), &elm);
}

/* STRINGIFY
 *
 * The idea behind the stringify is to be able to dump variable as a string
 * and use the result to check if it's equal with other dumped strings. We
 * need that for differential fuzzing, consider following example with simple
 * trace:
 *
 * 1 = get_zero()
 * 2 = get_one()
 * 3 = add(1,2)
 *
 * We execute this trace on two different versions of bignum library, then we
 * stringify variable 3 and compare. If we get the same result, then we're
 * good, if not we have to investigate because it's possible bug.
 *
 * char *minerva_var_stringify(minerva_type_t type, void *obj) returns string
 * representation of obj typed as type.
 *
 * Possible problems:
 *  - cyclic recursion
 *  - randomness
 *  - pointers
 */

char *minerva_var_stringify(minerva_type_t type, void *obj) {
    size_t i;

    for (i = 0; i < MINERVA_STRINGIFY_FUNC_NUM; i++) {
        if (minerva_var_stringify_funcs[i].type == type)
            return minerva_var_stringify_funcs[i].func(obj);
    }

    /* if we can't stringify then return null */
    return NULL;
}
