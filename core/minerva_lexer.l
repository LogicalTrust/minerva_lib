/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE"
 * If we meet some day, and you think this stuff is worth it, you can buy us 
 * a beer in return.
 * ----------------------------------------------------------------------------
 */

%{
#include <stdio.h>
#include <minerva_parser.h>
#include "minerva_parser.tab.h"
%}

%option noyywrap
%option noinput
%option nounput

blanks  [ \t\n\r]+
letter  [A-Za-z_]
digit   [0-9]
identifier  {letter}({letter}|{digit})*
number      {digit}+
string  "\""({letter}|{digit}|[ .-\/])*"\""
eq      "="
lparen  "("
rparen  ")"
comma   ","

%%

{eq}    {
        return (EQ);
}

{lparen}    {
        return (LPAREN);
}

{rparen}    {
        return (RPAREN);
}

{comma}    {
        return (COMMA);
}

{blanks}    {/*ign*/}

{identifier}    {
        yylval.sval = strdup(yytext);
        return (IDENTIFIER);
}

{string}    {
        yylval.sval = strdup(yytext);
        return (STRING);
}

{number}    {
        yylval.ival = atoi(yytext);
        return (NUMBER);
}
