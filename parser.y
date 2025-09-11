%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node* root = NULL;

extern int yylineno;
int yylex(void);
void yyerror(const char *s);
%}

/* Declaracion de tokens */
%union {
    Node* node;
    infoType t_Info;

    int int_num;
    char* str;
    int boolean;
}

%token  <int_num>   INT_NUM
%token  <str>       T_ID
%token  <boolean>   T_TRUE T_FALSE

%token  <t_Info>    T_VOID ID T_INTEGER T_BOOL

%nterm  <t_Info>    Type

%token              T_PROG T_EXTERN T_RETURN

%token              T_PLUS T_MINUS T_MULT T_DIVISION T_MOD

%token              T_LESS T_GREATER T_EQUAL

%token              T_NOT T_AND T_OR

%token              T_ASSIGN

%token              T_OPENP T_CLOSEP T_OPENB T_CLOSEB T_SEMIC T_COMMA

%type   <node>      T_IF T_THEN T_ELSE T_WHILE P VAR_DECL METHOD_DECL SENT EXP

/* Precedencia y asociatividad en Bison (arriba MENOS importante, abajo MAS importante) */
%left T_OR                      /* operador or logico ( || ) */
%left T_AND                     /* operador and logico ( && ) */
%left T_EQUAL                   /* operador igual a ( == ) */
%left T_LESS T_GREATER          /* operadores menor y mayor ( < > ) */
%left T_PLUS T_MINUS            /* operadores suma y resta ( + - ) */
%left T_MULT T_DIVISION T_MOD   /* operadores multiplicacion, division y modulo ( * / % ) */
%left T_NOT                     /* operador not logico ( ! ) */

%start P

%%
P : T_PROG T_OPENP VAR_DECL METHOD_DECL T_CLOSEB      { Value v = {0}; root = newNode_NonTerminal(PROG, $1, v, $6, $7); $$ = root; }
  ;

VAR_DECL : /* vacio */          { $$ = NULL; }
         | VAR_DECL DECL        { Value v = {0}; $$ = newNode_NonTerminal(DECL, NONE_INFO, v, $1, $2); }
         | DECL                 { $$ = $1; }
         ;

DECL : Type T_ID ';'    { Value v = {0}; Value v_id; v_id.id = $2; $$ = newNode_NonTerminal(DECL, NONE_INFO, v, newNode_Terminal($1, v), newNode_Terminal(TYPE_ID, v_id)); }
     ;

METHOD_DECL : METHOD_DECL SENT      { Value v = {0}; $$ = newNode_NonTerminal(SENT, NONE_INFO, v, $1, $2); }
            | SENT                  { $$ = $1; }
            ;

SENT : T_ID '=' EXP ';'     { Value v = {0}; Value v_id; v_id.id = $1; $$ = newNode_NonTerminal(ASSIGN, NONE_INFO, v, newNode_Terminal(TYPE_ID, v_id), $3); }
     | T_RET EXP ';'        { Value v = {0}; $$ = newNode_NonTerminal(RET, NONE_INFO, v, $2, NULL); }
     | T_RET ';'            { Value v = {0}; $$ = newNode_NonTerminal(RET, NONE_INFO, v, NULL, NULL); }
     ;

EXP : EXP '+' EXP       { Value v; v.bin_op = '+'; $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3); }
    | EXP '*' EXP       { Value v; v.bin_op = '*'; $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3); }
    | '(' EXP ')'       { $$ = $2; }
    | INT_NUM           { Value v; v.int_num = $1; $$ = newNode_Terminal(TYPE_INT, v); }
    | T_ID              { Value v; v.id = $1; $$ = newNode_Terminal(TYPE_ID, v); }
    | T_TRUE            { Value v; v.boolean = $1; $$ = newNode_Terminal(TYPE_BOOL, v); }
    | T_FALSE           { Value v; v.boolean = $1; $$ = newNode_Terminal(TYPE_BOOL, v); }
    ;

Type : T_VOID     { $$ = TYPE_VOID; }
     | T_INT      { $$ = TYPE_INT; }
     | T_BOOL     { $$ = TYPE_BOOL; }
     ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR en la linea %d: %s\n", yylineno, s);
}