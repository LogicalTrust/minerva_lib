/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minerva_colors.h>
#include <minerva_parser.h>
#include <minerva_repl.h>
#include <xmalloc.h>

#include "minerva_lexer.h"

int yylex(void);
void yyerror(const char *);

static minerva_line_t *call;
static int p_error = 0;
%}


%start ASSIGN

%define parse.error verbose

%union {
    int     ival;
    char    *sval;
    minerva_repl_arg_t *mvar;
    minerva_call_t *cvar;
    minerva_line_t *lvar;
}

%token EQ LPAREN RPAREN COMMA
%token <ival> NUMBER
%token <sval> STRING
%token <sval> IDENTIFIER
%type <mvar> ARG
%type <cvar> ARGS
%type <cvar> CALL
%type <lvar> ASSIGN
%type <mvar> VAR

%%
ASSIGN  :   VAR EQ CALL {
                $$ = xcalloc(1, sizeof(minerva_line_t));
                $$->var = $1;
                $$->call = $3;
                call = $$;
            }

        |   CALL {
                $$ = xcalloc(1, sizeof(minerva_line_t));
                $$->call = $1;
                call = $$;
            }
        ;

VAR     :   IDENTIFIER {
                $$ = xcalloc(1, sizeof(minerva_repl_arg_t));
                $$->type = VAR_VAL;
                $$->val.var = $1;
            }

        ;

CALL    :   IDENTIFIER LPAREN RPAREN {
                $$ = xcalloc(1, sizeof(minerva_call_t));
                $$->fname = $1;
            }
        |   IDENTIFIER LPAREN ARGS RPAREN {
                $$ = $3;
                $$->fname = $1;
            }
        |   IDENTIFIER {
                $$ = xcalloc(1, sizeof(minerva_call_t));
                $$->fname = $1;
            }
        ;

ARGS    :   ARGS COMMA ARG {
                $$ = $1;
                $$->num_arg++;
                $$->args = realloc($$->args, sizeof(minerva_repl_arg_t *) * $1->num_arg);
                $$->args[$1->num_arg - 1] = $3;
            }
        |   ARG {
                $$ = xcalloc(1, sizeof(minerva_call_t));
                $$->num_arg = 1;
                $$->args = xcalloc(1, sizeof(minerva_repl_arg_t *));
                $$->args[0] = $1;
            }
        ;

ARG     :   IDENTIFIER {
                $$ = xcalloc(1, sizeof(minerva_repl_arg_t));
                $$->type = VAR_VAL;
                $$->val.var = $1;
            }
        |   STRING { 
                $$ = xcalloc(1, sizeof(minerva_repl_arg_t));
                $$->type = STRING_VAL;
                /* trim quotes */
                $$->val.str = xstrdup(&($1[1]));
                $$->val.str[strlen($$->val.str)-1] = '\0';
            }
        |   NUMBER { 
                $$ = xcalloc(1, sizeof(minerva_repl_arg_t));
                $$->type = NUMBER_VAL;
                $$->val.num = $1;
            }
        ;
%%

void yyerror(const char *err) {
    p_error = 1;
    minerva_repl_error(err);
}

minerva_line_t *
minerva_repl_parse(const char *str)
{
    YY_BUFFER_STATE buffer;
    p_error = 0;
    buffer = yy_scan_string(str);
    yyparse();
    if (p_error != 0)
	return NULL;
    yy_delete_buffer(buffer);

    return call;
}

