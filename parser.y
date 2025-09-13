%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "ast.h"

// Node* root = NULL;

extern int yylineno;
int yylex(void);
void yyerror(const char *s);
%}

/* Declaracion de tokens */
%union {
    // Node* node;
    // infoType t_Info;

    int int_num;
    char* str;
    int boolean;
}

/* Tokens de terminales con tipo asociado */
%token  <int_num>   INT_NUM
%token  <str>       T_ID
%token  <boolean>   T_TRUE T_FALSE

%token  /* <t_Info> */    T_VOID T_INTEGER T_BOOL
%nterm  /* <t_Info> */    Type

/* Tokens de no terminales */
%token      T_PROG T_EXTERN T_RETURN
%token      T_PLUS T_MINUS T_MULT T_DIVISION T_MOD
%token      T_LESS T_GREATER T_EQUAL
%token      T_NOT T_AND T_OR
%token      T_ASSIGN
%token      T_OPENP T_CLOSEP T_OPENB T_CLOSEB T_SEMIC T_COMMA
%token      T_IF T_THEN T_ELSE T_WHILE

/* Tokens de no terminales con tipo asociado */
%type   /* <node> */      P VAR_DECLS METHOD_DECLS EXPR PARAMS

/* Precedencia y asociatividad en Bison (arriba MENOS importante, abajo MAS importante) */
%left   T_OR                      /* operador or logico ( || ) */
%left   T_AND                     /* operador and logico ( && ) */
%left   T_EQUAL                   /* operador igual a ( == ) */
%left   T_LESS T_GREATER          /* operadores menor y mayor ( < > ) */
%left   T_PLUS T_MINUS            /* operadores suma y resta ( + - ) */
%left   T_MULT T_DIVISION T_MOD   /* operadores multiplicacion, division y modulo ( * / % ) */
%left   T_NOT                     /* operador not logico ( ! ) */

%start P

%%
P : T_PROG T_OPENB T_CLOSEB                         { printf("program\n"); }
  | T_PROG T_OPENB VAR_DECLS T_CLOSEB               { printf("program\n"); }
  | T_PROG T_OPENB METHOD_DECLS T_CLOSEB            { printf("program\n"); }
  | T_PROG T_OPENB VAR_DECLS METHOD_DECLS T_CLOSEB  { printf("program\n"); }
  ;

VAR_DECLS : VAR_DECLS VAR_DECL      { printf("decl\n"); }
          | VAR_DECL                { printf("decl\n"); }
          ;

VAR_DECL : Type T_ID T_ASSIGN EXPR T_SEMIC       { printf(" %s = ", $2); }
         ;

METHOD_DECLS : METHOD_DECLS METHOD_DECL      { printf("method\n"); }
             | METHOD_DECL                   { printf("method\n"); }
             ;

METHOD_DECL : Type T_ID T_OPENP PARAMS T_CLOSEP BLOCK_OR_EXTERN         { printf(" %s (", $2); }
            | T_VOID T_ID T_OPENP PARAMS T_CLOSEP BLOCK_OR_EXTERN       { printf(" %s (", $2); }
            ;

PARAMS : /* vacio */        {  }
       | PARAMS T_COMMA     { printf(", "); }
       | Type T_ID          { printf(" %s", $2); }
       ;

BLOCK_OR_EXTERN : BLOCK     {  }
                | T_EXTERN T_SEMIC      { printf("extern"); }
                ;

BLOCK : T_OPENB T_CLOSEB                        { printf("block\n"); }
      | T_OPENB VAR_DECLS T_CLOSEB              { printf("block\n"); }
      | T_OPENB STATEMENTS T_CLOSEB             { printf("block\n"); }
      | T_OPENB VAR_DECLS STATEMENTS T_CLOSEB   { printf("block\n"); }
      ;

Type : T_INTEGER    { printf("integer "); }
     | T_BOOL       { printf("bool "); }
     ;

STATEMENTS : STATEMENTS STATEMENT   { printf("statement\n"); }
           | STATEMENT              { printf("statement\n"); }
           ;

STATEMENT : T_ID T_ASSIGN EXPR T_SEMIC                          { printf("%s = ", $1); }
          | METHOD_CALL T_SEMIC                                 { printf("method_call\n"); }
          | T_IF T_OPENP EXPR T_CLOSEP T_THEN BLOCK ELSE_ST     { printf("if ("); }
          | T_WHILE EXPR BLOCK                                  { printf("while "); }
          | T_RETURN EXPR_ST T_SEMIC        { printf("return "); }
          | T_SEMIC     { printf(";"); }
          | BLOCK       {  }
          ;

ELSE_ST : /* vacio */       {  }
        | T_ELSE BLOCK      { printf("else "); }
        ;

EXPR_ST : /* vacio */       {  }
        | EXPR              { printf("expr\n"); }
        ;

METHOD_CALL : T_ID T_OPENP EXPRS T_CLOSEP      { printf("%s (", $1); }
            ;

EXPRS : /* vacio */         {  }
      | EXPRS T_COMMA       { printf(", "); }
      | EXPR                {  }
      ;

EXPR : T_ID                     { printf("%s", $1); }
     | METHOD_CALL              { printf("method_call\n"); }
     | LITERAL                  {  }
     | EXPR T_PLUS EXPR         { printf(" + "); }
     | EXPR T_MINUS EXPR        { printf(" - "); }
     | EXPR T_MULT EXPR         { printf(" * "); }
     | EXPR T_DIVISION EXPR     { printf(" / "); }
     | EXPR T_MOD EXPR          { printf(" % "); }
     | EXPR T_LESS EXPR         { printf(" < "); }
     | EXPR T_GREATER EXPR      { printf(" > "); }
     | EXPR T_EQUAL EXPR        { printf(" == "); }
     | EXPR T_AND EXPR          { printf(" && "); }
     | EXPR T_OR EXPR           { printf(" || "); }
     | T_MINUS EXPR             { printf("-"); }
     | T_NOT EXPR               { printf("!"); }
     | T_OPENP EXPR T_CLOSEP    { printf("parentesis\n"); }
     ;

LITERAL : INT_NUM   { printf("%d", $1); }
        | T_TRUE    { printf("true"); }
        | T_FALSE   { printf("false"); }
        ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR en la linea %d: %s\n", yylineno, s);
}