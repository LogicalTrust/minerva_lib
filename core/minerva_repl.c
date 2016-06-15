/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <minerva_loop.h>
#include <minerva_colors.h>
#include <minerva_repl.h>
#include <minerva_func.h>
#include <minerva_parser.h>
#include <minerva_trace.h>
#include <minerva_dict.h>
#include <xmalloc.h>

int verbose = VERBOSE_NORMAL;

static minerva_dict_var_t *
repl_help(int argc, minerva_dict_var_t **args)
{
    printf(""
      "fuzz([iter, [ctx]]) - fuzz: does iter iterations, uses ctx\n"
      "var_mut(iter, ctx) - does iter mutations of variables stored in ctx\n" 
      "var_stat(ctx) - shows statistics about variables stored in ctx\n"
      "func_stat(ctx) - shows statistics about functions calls stored in ctx\n"
      "play(trace) - plays trace or context\n"
      "restore(\"filename\") - restores trace from file\n"
      "save(trace, \"filename\") - saves trace to file\n"
      "min(trace) - minimizes trace\n"
      "show(trace) - shows trace\n"
      "verbose(num) - set verbose level 0 - normal, 1 - noisy, 2+ debug\n"
      "quit, exit - quits\n");

    return NULL;
}

static minerva_dict_var_t *
repl_fuzz(int argc, minerva_dict_var_t **args)
{
    minerva_vars_t *vars = NULL;
    minerva_funcs_t *funcs = NULL;
    minerva_trace_t *trace = NULL;
    minerva_dict_var_t *r;
    minerva_dict_var_result_t *result; 
    unsigned int iter;

    if (argc > 0) {
        if (args[0]->type != MINERVA_NUMBER_T) {
            minerva_repl_error("type mismatch");
            return NULL;
        }
        iter = args[0]->val.num;
    } else
        iter = 0;

    if (argc > 1) {
        if (args[1]->type != MINERVA_RESULT_T) {
            minerva_repl_error("type mismatch");
            return NULL;
        }

        result = args[1]->val.var;
        vars = result->vars;
        funcs = result->funcs;
        trace = result->trace;
    } else
        result = xcalloc(1, sizeof(*result));

    MINERVA_REFS_INC(result);
    r = xcalloc(1, sizeof(*r));
    r->type = MINERVA_RESULT_T;
    r->val.var = result;

    printf(ANSI_CURSOR_HIDE);
    minerva_loop(iter, &vars, &funcs, &trace, 0);
    printf(ANSI_CURSOR_SHOW);

    result->vars = vars;
    result->funcs = funcs;
    result->trace = trace;

    return r;
}

static minerva_dict_var_t *
repl_verbose(int argc, minerva_dict_var_t **args)
{
    if (args[0]->type != MINERVA_NUMBER_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    verbose = args[0]->val.num;

    return NULL;
}


static minerva_dict_var_t *
repl_quit(int argc, minerva_dict_var_t **args)
{
    exit(EXIT_SUCCESS);
    return NULL;
}

static minerva_dict_var_t *
repl_play(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_result_t *result;
    
    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[0]->val.var;

    switch(minerva_trace_play(result->trace)) {
        case R_PLAY_NOTCRASHED:
            minerva_repl_error(
              "not crashed :(");
            break;
        case R_PLAY_CRASHEDMIDDLE:
            minerva_repl_error(
              "crashed in the middle!");
            break;
        case R_PLAY_CRASHEDLAST:
            printf(ANSI_COLOR_GREEN "\\o/ crashed \\o/" ANSI_COLOR_RESET "\n" );
            break;
        default:
        /*NOTREACHED*/
        assert(0);
    }


    return NULL;
}

static minerva_dict_var_t *
repl_restore(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_t *r;
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_STRING_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    r = xcalloc(1, sizeof(*r));
    result = xcalloc(1, sizeof(*result));
    MINERVA_REFS_INC(result);
    r->type = MINERVA_RESULT_T;
    r->val.var = result;
    result->funcs =  minerva_funcs_new();
    minerva_funcs_init(result->funcs);

    result->trace = minerva_trace_restore(args[0]->val.str, result->funcs);

    return r;
}

static minerva_dict_var_t *
repl_show(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[0]->val.var;

    minerva_trace_save(result->trace, "/dev/stdout");

    return NULL;
   
}

static minerva_dict_var_t *
repl_save(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    if (args[1]->type != MINERVA_STRING_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[0]->val.var;

    minerva_trace_save(result->trace, args[1]->val.str);

    return NULL;
   
}

static minerva_dict_var_t *
repl_stat_func(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[0]->val.var;

    minerva_funcs_print_stats(result->funcs); 

    return NULL;
}

static minerva_dict_var_t *
repl_var_mut(int argc, minerva_dict_var_t **args)
{
    minerva_vars_t *vars = NULL;
    minerva_funcs_t *funcs = NULL;
    minerva_trace_t *trace = NULL;
    minerva_dict_var_result_t *result; 
    unsigned int iter;

    if (args[0]->type != MINERVA_NUMBER_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }
    iter = args[0]->val.num;

    if (args[1]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[1]->val.var;
    vars = result->vars;
    funcs = result->funcs;
    trace = result->trace;

    printf(ANSI_CURSOR_HIDE);
    minerva_loop(iter, &vars, &funcs, &trace, 1);
    printf(ANSI_CURSOR_SHOW);

    return NULL;
}

static minerva_dict_var_t *
repl_stat_var(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    result = args[0]->val.var;

    minerva_vars_print_stats(result->vars); 

    return NULL;
}

static minerva_dict_var_t *
repl_min(int argc, minerva_dict_var_t **args)
{
    minerva_dict_var_t *r;
    minerva_dict_var_result_t *result;

    if (args[0]->type != MINERVA_RESULT_T) {
        minerva_repl_error("type mismatch");
        return NULL;
    }

    r = xcalloc(1, sizeof(*r));
    result = xcalloc(1, sizeof(*result));
    MINERVA_REFS_INC(result);
    r->type = MINERVA_RESULT_T;
    r->val.var = result;
    result->funcs =  minerva_funcs_new();
    minerva_funcs_init(result->funcs);

    result->trace = minerva_trace_minimize(
      ((minerva_dict_var_result_t*)(args[0]->val.var))->trace, result->funcs);

    switch(minerva_trace_play(result->trace)) {
        case R_PLAY_NOTCRASHED:
            minerva_repl_error(
              "whoops, after minimization the trace doesn't crash anymore");
            break;
        case R_PLAY_CRASHEDMIDDLE:
            minerva_repl_error(
              "whoops, after minimization the trace crashes in the middle");
            break;
        case R_PLAY_CRASHEDLAST:
            printf(ANSI_COLOR_GREEN "all good" ANSI_COLOR_RESET "\n" );
            break;
        default:
        /*NOTREACHED*/
        assert(0);
    }


    return r;
}

static minerva_repl_funcs_t repl_funcs[] = {
    {"var_mut", repl_var_mut, 2}, 
    {"var_stat", repl_stat_var, 1},
    {"func_stat", repl_stat_func, 1},
    {"play", repl_play, 1},
    {"restore", repl_restore, 1},
    {"save", repl_save, 1},
    {"min", repl_min, 1},
    {"quit", repl_quit, 0},
    {"exit", repl_quit, 0},
    {"show", repl_show, 1},
    {"fuzz", repl_fuzz, 0},
    {"help", repl_help, 0},
    {"verbose", repl_verbose, 1},
    {NULL, NULL, 0}
};

void
minerva_repl_error(const char *err)
{
    fprintf(stderr, ANSI_COLOR_YELLOW "%s\n" ANSI_COLOR_RESET, err);
}

static void
minerva_repl_arg_destroy(minerva_repl_arg_t *arg)
{
    if (arg == NULL)
        return;

    switch (arg->type) {
        case VAR_VAL:
            free(arg->val.var);
            break;
        case STRING_VAL:
            free(arg->val.str);
            break;
        case NUMBER_VAL:
            /* do nothing */
            break;
        default:
            assert(0); /* IMPOSSIBLE */
    }

    free(arg);
}

static void
minerva_repl_call_destroy(minerva_call_t *call)
{
    int i;

    if (call == NULL)
        return;

    free(call->fname);

    for (i = 0; i < call->num_arg; i++)
        minerva_repl_arg_destroy(call->args[i]);

    free(call->args);
    free(call);
}

void
minerva_repl_line_destroy(minerva_line_t *line)
{
    if (line == NULL)
        return;
    minerva_repl_arg_destroy(line->var);
    minerva_repl_call_destroy(line->call);
    free(line);
}

void
minerva_repl_eval_line(dict_t *vars, minerva_line_t *line)
{
    int i, notfound = 1;
    minerva_dict_var_t **argv;
    minerva_dict_var_t *result = NULL;

    if (line->call == NULL)
        return;

    /* conver repl_arg -> dict_var */
    argv = xcalloc(line->call->num_arg, sizeof(*argv));

    for (i = 0; i < line->call->num_arg; i++) {
        switch (line->call->args[i]->type) {
            case NUMBER_VAL:
            case STRING_VAL:
                argv[i] = xcalloc(1, sizeof(**argv));
                if (line->call->args[i]->type == NUMBER_VAL) {
                    argv[i]->type = MINERVA_NUMBER_T;
                    argv[i]->val.num = line->call->args[i]->val.num;
                } else {
                    argv[i]->type = MINERVA_STRING_T;
                    argv[i]->val.str = xstrdup(line->call->args[i]->val.str);
                }
                break;
            case VAR_VAL:
                argv[i] = minerva_dict_get(vars,
                  line->call->args[i]->val.var);
                if (argv[i] == NULL) {
                    minerva_repl_error("no such var");
                    goto error; 
                }
                break;
            default:
                assert(0); /*IMPOSSIBLE*/
        }
    }

    for (i = 0; repl_funcs[i].faddr != NULL; i++) {
        if (strcasecmp(line->call->fname, repl_funcs[i].fname) == 0) {
            notfound = 0;
            if (line->call->num_arg < repl_funcs[i].num_arg)
                minerva_repl_error("args no mismatch");
            else
                result = repl_funcs[i].faddr(line->call->num_arg, argv);
        }
    }

    if (notfound != 0) {
        minerva_repl_error("function not found");
    }

    if (line->var != NULL) {
        if (result != NULL)
            minerva_dict_set(vars, line->var->val.var, result);
        else
            minerva_repl_error("no result");
    } else {
        if (result != NULL)
            minerva_dict_set(vars, "_", result);
    }

error:
    for (i = 0; i < line->call->num_arg; i++) {
        if (argv[i] == NULL)
            break;
        switch (argv[i]->type) {
            case MINERVA_NUMBER_T:
            case MINERVA_STRING_T:
                free(argv[i]);
                break;
            default:
                /* do nothing */
                break;
        }
    }
    free(argv);

    return;
}

void
minerva_repl(void)
{
    char *input;
    minerva_line_t *line;
    dict_t *vars;

    vars = minerva_dict_create();

    for(;;) {
        input = readline(ANSI_COLOR_RED "pfl> " ANSI_COLOR_RESET);
        if (!input)
            break;
        if (strlen(input) > 0) {
            line = minerva_repl_parse(input);
            if (line != NULL)
                minerva_repl_eval_line(vars, line);
            minerva_repl_line_destroy(line);
            add_history(input);
        }
        free(input);
    }

    printf("\n");
    minerva_dict_destroy(vars);

    return;
}
