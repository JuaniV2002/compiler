%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "parser.tab.h"

int comment_start_line = 0;
%}

%option noyywrap
%option yylineno

/* Declaracion de estado exclusivo para comentarios */
%x COMMENT

/* Macros para definir reglas de la gramatica */
DIGIT       [0-9]
ALPHA       [a-zA-Z]

/* Reglas de la gramatica */
%%
"//"[^\n]*        { /* ignorar comentario de línea */ }
"/*"              { BEGIN(COMMENT); comment_start_line = yylineno; /* iniciar estado de comentario y guarda el numero de linea */ }

<COMMENT>"*/"     { BEGIN(INITIAL); /* finalizar estado de comentario e inicia estado por defecto */ }
<COMMENT>\n       { }
<COMMENT>.        { /* ignorar cualquier otro caracter dentro del comentario */ }
<COMMENT><<EOF>>  { fprintf(stderr, "Error: comentario no cerrado (linea %d)\n", comment_start_line); exit(EXIT_FAILURE); }

[ \t\r\n]+      { /* ignorar espacios en blanco */ }

-?{DIGIT}+      { long val = strtol(yytext, NULL, 10);      /* Numeros enteros entre el minimo y maximo (−2147483648 y 2147483647) */
                if (val < INT_MIN || val > INT_MAX) {
                    fprintf(stderr, "Error: número fuera de rango (%s)\n", yytext);
                    fprintf(stderr, "       (linea %d)\n", yylineno);
                    exit(EXIT_FAILURE);
                }
                yylval.int_num = (int) val; return INT_NUM; }

"program"   { return T_PROG; }      /*  Palabras reservadas con sus tokens */
"extern"    { return T_EXTERN; }
"void"      { return T_VOID; }
"integer"   { return T_INTEGER; }
"bool"      { return T_BOOL; }
"true"      { yylval.boolean = 1; return T_TRUE; }
"false"     { yylval.boolean = 0; return T_FALSE; }
"if"        { return T_IF; }
"then"      { return T_THEN; }
"else"      { return T_ELSE; }
"while"     { return T_WHILE; }
"return"    { return T_RETURN; }

{ALPHA}({ALPHA}|{DIGIT}|[_])*    { yylval.str = strdup(yytext); return T_ID; }      /* Identificadores */

"+"     { return T_PLUS; }      /* Operadores aritmeticos y logicos con sus tokens */
"-"     { return T_MINUS; }
"*"     { return T_MULT; }
"/"     { return T_DIVISION; }           /* (TODO) cambiar los return por tokens y tratarlos como enum en ast */
"%"     { return T_MOD; }

"<"     { return T_LESS; }
">"     { return T_GREATER; }
"=="    { return T_EQUAL; }

"!"     { return T_NOT; }
"&&"    { return T_AND; }
"||"    { return T_OR; }

"="     { return T_ASSIGN; }    /* Operador de asignacion */

"("     { return T_OPENP; }     /* Simbolos de agrupacion y separacion con sus tokens */
")"     { return T_CLOSEP; }
"{"     { return T_OPENB; }
"}"     { return T_CLOSEB; }

","     { return T_COMMA; }
";"     { return T_SEMIC; }
%%