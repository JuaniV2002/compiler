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

%token  <t_Info>    T_VOID ID T_INT T_BOOL

%nterm  <t_Info>    Type

%token              T_MAIN T_RET

%type   <node>      P DECLS DECL SENTS SENT EXP

/* Orden de mas precedencia a menos */
/* () {} [] fraccion raiz      expon   multip   divis   suma    resta */
/* not      and     or */

/* Precedencia y asociatividad en Bison (arriba MENOS importante, abajo MAS importante) */
%left '+'     /* operador +, asociativo a izquierda */
%left '*'     /* operador *, asociativo a izquierda */

%start P

%%
P : Type T_MAIN '(' ')' '{' DECLS SENTS '}'     { Value v = {0}; root = newNode_NonTerminal(PROG, $1, v, $6, $7); $$ = root; }
  ;

DECLS : DECLS DECL      { Value v = {0}; $$ = newNode_NonTerminal(DECL, NONE_INFO, v, $1, $2); }
      | DECL            { $$ = $1; }
      ;

DECL : Type T_ID ';'    { Value v = {0}; Value v_id; v_id.id = $2; $$ = newNode_NonTerminal(DECL, NONE_INFO, v, newNode_Terminal($1, v), newNode_Terminal(TYPE_ID, v_id)); }
     ;

SENTS : SENTS SENT      { Value v = {0}; $$ = newNode_NonTerminal(SENT, NONE_INFO, v, $1, $2); }
      | SENT            { $$ = $1; }
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