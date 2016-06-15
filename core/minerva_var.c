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

#ifdef MINERVA_DEBUG
    for (i = 0; i < __minerva_types_no; i++) {
        minerva_assert(vars->len[i] == 0);
    }
#endif

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
    elm.id = rand() % (var->id+1);
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
