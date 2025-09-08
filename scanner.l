%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ast.h"
#include "analiz_sintactico.tab.h"
%}

%option noyywrap
%option yylineno
/* Macros para definir tokens */
DIGIT       [0-9]
ALPHA      [a-zA-Z]
LINECOM     "//".*
BLOCKCOM    "/*"([^*]|\*+[^*/])*\*+"/"

/* Reglas de la gramatica */
%%
{LINECOM}       { /* ignorar comentarios de linea */ }
{BLOCKCOM}      { /* ignorar comentarios de bloque */ }
[ \t\r\n]+      { /* ignorar espacios en blanco */ }
-?{DIGIT}+      { long val = strtol(yytext, NULL, 10);
                if (val < INT_MIN || val > INT_MAX) {
                    fprintf(stderr, "Error: número fuera de rango (%s)\n", yytext);
                    exit(1);
                }
                yylval.int_num = (int) val; return INT_NUM; }

"return"    { return T_RETURN; }
"integer"   { return T_INTEGER; }
"bool"      { return T_BOOL; }
"void"      { return T_VOID; }
"extern"    { return T_EXTERN; }
"program"   { return T_PROG; }
"else"      { return T_ELSE; }
"then"      { return T_THEN; }
"if"        { return T_IF; }
"while"     { return T_WHILE; }
"true"      { yylval.boolean = 1; return T_TRUE; }
"false"     { yylval.boolean = 0; return T_FALSE; }

{ALPHA}({ALPHA}|{DIGIT}|[_])*    { yylval.str = strdup(yytext); return T_ID; }

"+"     { return T_PLUS; }  /* (TODO) cambiar los return por tokens y tratarlos como enum en ast */
"-"     { return T_MINUS; }
"*"     { return T_MULT; }
"/"     { return T_DIVISION; }
"%"     { return T_MOD; }

"<"     { return T_LESS; }
">"     { return T_GREATER; }
"=="     { return T_EQ; }

"&&"     { return T_AND; }
"||"     { return T_OR; }

"="     { return T_ASSIGN; }

"("     { return T_OPENP; }
")"     { return T_CLOSEP; }
"{"     { return T_OPENB; }
"}"     { return T_CLOSEB; }

";"     { return T_SEMIC; }
%%