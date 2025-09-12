%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);

/* Provide yywrap to avoid linking with -lfl */
int yywrap(void) { return 1; }
%}

/* Solo los tipos necesarios para tokens con valor */
%union {
    int   int_num;
    char* str;
    int   boolean;
}

/* Tokens con valor */
%token  <int_num>   INT_NUM
%token  <str>       T_ID
%token  <boolean>   T_TRUE T_FALSE

/* Resto de tokens (sin valor) */
%token  T_VOID T_INTEGER T_BOOL
%token  T_PROG T_EXTERN T_RETURN
%token  T_IF T_THEN T_ELSE T_WHILE
%token  T_PLUS T_MINUS T_MULT T_DIVISION T_MOD
%token  T_LESS T_GREATER T_EQUAL
%token  T_NOT T_AND T_OR
%token  T_ASSIGN
%token  T_OPENP T_CLOSEP T_OPENB T_CLOSEB T_SEMIC T_COMMA

%start P

%%
/* Acepta cualquier secuencia de tokens del scanner */
P       : stream
        ;

stream  : /* empty */
        | stream tok
        ;

tok     : INT_NUM
        | T_ID
        | T_TRUE
        | T_FALSE
        | T_VOID
        | T_INTEGER
        | T_BOOL
        | T_PROG
        | T_EXTERN
        | T_RETURN
        | T_IF
        | T_THEN
        | T_ELSE
        | T_WHILE
        | T_PLUS
        | T_MINUS
        | T_MULT
        | T_DIVISION
        | T_MOD
        | T_LESS
        | T_GREATER
        | T_EQUAL
        | T_NOT
        | T_AND
        | T_OR
        | T_ASSIGN
        | T_OPENP
        | T_CLOSEP
        | T_OPENB
        | T_CLOSEB
        | T_SEMIC
        | T_COMMA
        ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR: %s\n", s);
}

int main (void) {
    return yyparse();
}