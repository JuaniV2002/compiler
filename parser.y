%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
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

/* Tokens de terminales con tipo asociado */
%token  <int_num>   INT_NUM
%token  <str>       T_ID
%token  <boolean>   T_TRUE T_FALSE

%token  <t_Info>    T_VOID T_INTEGER T_BOOL
%type   <t_Info>    Type

/* Tokens de no terminales */
%token      T_PROG T_EXTERN T_RETURN
%token      T_PLUS T_MINUS T_MULT T_DIVISION T_MOD
%token      T_LESS T_GREATER T_EQUAL
%token      T_NOT T_AND T_OR
%token      T_ASSIGN
%token      T_OPENP T_CLOSEP T_OPENB T_CLOSEB T_SEMIC T_COMMA
%token      T_IF T_THEN T_ELSE T_WHILE

/* Tokens de no terminales con tipo asociado */
%type   <node>      PROG VAR_DECLS VAR_DECL METHOD_DECLS METHOD_DECL EXPR PARAMS 
%type   <node>      BLOCK_OR_EXTERN STATEMENTS STATEMENT BLOCK ELSE_ST EXPR_ST METHOD_CALL EXPRS LITERAL

/* Precedencia y asociatividad en Bison (arriba MENOS importante, abajo MAS importante) */
%left   T_OR                      /* operador or logico ( || ) */
%left   T_AND                     /* operador and logico ( && ) */
%left   T_EQUAL                   /* operador igual a ( == ) */
%left   T_LESS T_GREATER          /* operadores menor y mayor ( < > ) */
%left   T_PLUS T_MINUS            /* operadores suma y resta ( + - ) */
%left   T_MULT T_DIVISION T_MOD   /* operadores multiplicacion, division y modulo ( * / % ) */
%right  T_NOT                     /* operador not logico ( ! ) */
%right  U_MINUS                   /* operador unario negacion ( - ) */

%start PROG

%%
PROG : T_PROG T_OPENB T_CLOSEB { 
            Value v = {0}; 
            root = newNode_NonTerminal(PROG, NONE_INFO, v, NULL, NULL, NULL); 
            $$ = root; 
        }
    | T_PROG T_OPENB VAR_DECLS T_CLOSEB { 
            Value v = {0}; 
            root = newNode_NonTerminal(PROG, NONE_INFO, v, $3, NULL, NULL); 
            $$ = root; 
        }
    | T_PROG T_OPENB METHOD_DECLS T_CLOSEB { 
            Value v = {0}; 
            root = newNode_NonTerminal(PROG, NONE_INFO, v, NULL, $3, NULL); 
            $$ = root; 
        }
    | T_PROG T_OPENB VAR_DECLS METHOD_DECLS T_CLOSEB { 
            Value v = {0}; 
            root = newNode_NonTerminal(PROG, NONE_INFO, v, $3, $4, NULL); 
            $$ = root; 
        }
    ;

VAR_DECLS : VAR_DECLS VAR_DECL { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(DECL, NONE_INFO, v, $1, $2, NULL); 
    }
          | VAR_DECL { 
        $$ = $1; 
    }
          ;

VAR_DECL : Type T_ID T_ASSIGN EXPR T_SEMIC { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($2); 
        Node* typeNode = newNode_Terminal($1, v);
        Node* idNode = newNode_Terminal(TYPE_ID, v_id);
        $$ = newNode_NonTerminal(DECL, NONE_INFO, v, typeNode, idNode, $4); 
    }
         ;

METHOD_DECLS : METHOD_DECLS METHOD_DECL { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(METHOD, NONE_INFO, v, $1, $2, NULL); 
    }
             | METHOD_DECL { 
        $$ = $1; 
    }
             ;

METHOD_DECL : Type T_ID T_OPENP PARAMS T_CLOSEP BLOCK_OR_EXTERN { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($2); 
        Node* typeNode = newNode_Terminal($1, v);
        Node* idNode = newNode_Terminal(TYPE_ID, v_id);
        Node* methodNode = newNode_NonTerminal(METHOD, NONE_INFO, v, typeNode, idNode, $4);
        $$ = newNode_NonTerminal(METHOD, NONE_INFO, v, methodNode, $6, NULL); 
    }
            | T_VOID T_ID T_OPENP PARAMS T_CLOSEP BLOCK_OR_EXTERN { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($2); 
        Node* typeNode = newNode_Terminal(TYPE_VOID, v);
        Node* idNode = newNode_Terminal(TYPE_ID, v_id);
        Node* methodNode = newNode_NonTerminal(METHOD, NONE_INFO, v, typeNode, idNode, $4);
        $$ = newNode_NonTerminal(METHOD, NONE_INFO, v, methodNode, $6, NULL); 
    }
            ;

PARAMS : /* vacio */ { 
        $$ = NULL; 
    }
       | PARAMS T_COMMA Type T_ID { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($4); 
        Node* typeNode = newNode_Terminal($3, v);
        Node* idNode = newNode_Terminal(TYPE_ID, v_id);
        Node* paramNode = newNode_NonTerminal(PARAM, NONE_INFO, v, typeNode, idNode, NULL);
        $$ = newNode_NonTerminal(PARAMS, NONE_INFO, v, $1, paramNode, NULL); 
    }
       | Type T_ID { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($2); 
        Node* typeNode = newNode_Terminal($1, v);
        Node* idNode = newNode_Terminal(TYPE_ID, v_id);
        $$ = newNode_NonTerminal(PARAM, NONE_INFO, v, typeNode, idNode, NULL); 
    }
       ;

BLOCK_OR_EXTERN : BLOCK { 
        $$ = $1; 
    }
                | T_EXTERN T_SEMIC { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(EXTERN, NONE_INFO, v, NULL, NULL, NULL); 
    }
                ;

BLOCK : T_OPENB T_CLOSEB { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(BLOCK, NONE_INFO, v, NULL, NULL, NULL); 
    }
      | T_OPENB VAR_DECLS T_CLOSEB { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(BLOCK, NONE_INFO, v, $2, NULL, NULL); 
    }
      | T_OPENB STATEMENTS T_CLOSEB { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(BLOCK, NONE_INFO, v, NULL, $2, NULL); 
    }
      | T_OPENB VAR_DECLS STATEMENTS T_CLOSEB { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(BLOCK, NONE_INFO, v, $2, $3, NULL); 
    }
      ;

Type : T_INTEGER { 
        $$ = TYPE_INTEGER; 
    }
     | T_BOOL { 
        $$ = TYPE_BOOL; 
    }
     ;

STATEMENTS : STATEMENTS STATEMENT { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(STATEMENT, NONE_INFO, v, $1, $2, NULL); 
    }
           | STATEMENT { 
        $$ = $1; 
    }
           ;

STATEMENT : T_ID T_ASSIGN EXPR T_SEMIC { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($1); 
        $$ = newNode_NonTerminal(ASSIGN, NONE_INFO, v, newNode_Terminal(TYPE_ID, v_id), $3, NULL); 
    }
          | METHOD_CALL T_SEMIC { 
        $$ = $1; 
    }
          | T_IF T_OPENP EXPR T_CLOSEP T_THEN BLOCK ELSE_ST { 
        Value v = {0}; 
        $$ = newNode_NonTerminal3(IF, NONE_INFO, v, $3, $6, $7); 
    }
          | T_WHILE EXPR BLOCK { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(WHILE, NONE_INFO, v, $2, $3, NULL); 
    }
          | T_RETURN EXPR_ST T_SEMIC { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(RET, NONE_INFO, v, $2, NULL, NULL); 
    }
          | T_SEMIC { 
        $$ = NULL; 
    }
          | BLOCK { 
        $$ = $1; 
    }
          ;

ELSE_ST : /* vacio */ { 
        $$ = NULL; 
    }
        | T_ELSE BLOCK { 
        $$ = $2; 
    }
        ;

EXPR_ST : /* vacio */ { 
        $$ = NULL; 
    }
        | EXPR { 
        $$ = $1; 
    }
        ;

METHOD_CALL : T_ID T_OPENP EXPRS T_CLOSEP { 
        Value v = {0}; 
        Value v_id; 
        v_id.id = strdup($1); 
        $$ = newNode_NonTerminal(METHOD_CALL, NONE_INFO, v, newNode_Terminal(TYPE_ID, v_id), $3, NULL); 
    }
            ;

EXPRS : /* vacio */ { 
        $$ = NULL; 
    }
      | EXPRS T_COMMA EXPR { 
        Value v = {0}; 
        $$ = newNode_NonTerminal(EXPRS, NONE_INFO, v, $1, $3, NULL); 
    }
      | EXPR { 
        $$ = $1; 
    }
      ;

EXPR : T_ID { 
        Value v; 
        v.id = strdup($1); 
        $$ = newNode_Terminal(TYPE_ID, v); 
    }
     | METHOD_CALL { 
        $$ = $1; 
    }
     | LITERAL { 
        $$ = $1; 
    }
     | EXPR T_PLUS EXPR { 
        Value v; 
        v.bin_op = T_PLUS; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_MINUS EXPR { 
        Value v; 
        v.bin_op = T_MINUS; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_MULT EXPR { 
        Value v; 
        v.bin_op = T_MULT; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_DIVISION EXPR { 
        Value v; 
        v.bin_op = T_DIVISION; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_MOD EXPR { 
        Value v; 
        v.bin_op = T_MOD; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_LESS EXPR { 
        Value v; 
        v.bin_op = T_LESS; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_GREATER EXPR { 
        Value v; 
        v.bin_op = T_GREATER; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_EQUAL EXPR { 
        Value v; 
        v.bin_op = T_EQUAL; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_AND EXPR { 
        Value v; 
        v.bin_op = T_AND; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | EXPR T_OR EXPR { 
        Value v; 
        v.bin_op = T_OR; 
        $$ = newNode_NonTerminal(EXP, TYPE_BIN_OP, v, $1, $3, NULL); 
    }
     | T_MINUS EXPR %prec U_MINUS { 
        Value v; 
        v.un_op = T_UN_MINUS; 
        $$ = newNode_NonTerminal(EXP, TYPE_UN_OP, v, $2, NULL, NULL); 
    }
     | T_NOT EXPR { 
        Value v; 
        v.un_op = T_UN_NOT; 
        $$ = newNode_NonTerminal(EXP, TYPE_UN_OP, v, $2, NULL, NULL); 
    }
     | T_OPENP EXPR T_CLOSEP { 
        $$ = $2; 
    }
     ;

LITERAL : INT_NUM { 
        Value v; 
        v.int_num = $1; 
        $$ = newNode_Terminal(TYPE_INTEGER, v); 
    }
        | T_TRUE { 
        Value v; 
        v.boolean = 1; 
        $$ = newNode_Terminal(TYPE_BOOL, v); 
    }
        | T_FALSE { 
        Value v; 
        v.boolean = 0; 
        $$ = newNode_Terminal(TYPE_BOOL, v); 
    }
        ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR en la linea %d: %s\n", yylineno, s);
}