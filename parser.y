%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"

Node* root = NULL;
Symbol* currentMethod = NULL;

extern int yylineno;
int yylex(void);
void yyerror(const char *s);
%}

/* Declaracion de tokens */
%union {
    struct Node* node;
    struct Symbol* symbol;
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
%type   <symbol>    PARAMS METHOD_PROFILE
%type   <node>      PROG VAR_DECLS VAR_DECL METHOD_DECLS METHOD_DECL EXPR
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
                                    root = newNode_NonTerminal(N_PROG, NULL, NULL, NULL, NULL);
                                    root->lineNo = yylineno;
                                    $$ = root;
                                }
     | T_PROG T_OPENB VAR_DECLS T_CLOSEB {
                                            root = newNode_NonTerminal(N_PROG, NULL, $3, NULL, NULL);
                                            root->lineNo = yylineno;
                                            $$ = root;
                                        }
     | T_PROG T_OPENB METHOD_DECLS T_CLOSEB {
                                                root = newNode_NonTerminal(N_PROG, NULL, NULL, $3, NULL);
                                                root->lineNo = yylineno;
                                                $$ = root;
                                            }
     | T_PROG T_OPENB VAR_DECLS METHOD_DECLS T_CLOSEB {
                                                        root = newNode_NonTerminal(N_PROG, NULL, $3, $4, NULL);
                                                        root->lineNo = yylineno;
                                                        $$ = root;
                                                    }
     ;

VAR_DECLS : VAR_DECLS VAR_DECL {
                                    Node* node = newNode_NonTerminal(N_VAR_DECL, NULL, $1, $2, NULL);
                                    node->lineNo = yylineno;
                                    $$ = node;
                                }
          | VAR_DECL { $$ = $1; }
          ;

VAR_DECL : Type T_ID T_ASSIGN EXPR T_SEMIC {
                                                Symbol* sym = newSymbol(VAR, $1, $2, 0);
                                                Node* node = newNode_NonTerminal(N_VAR_DECL, sym, $4, NULL, NULL);
                                                node->lineNo = yylineno;
                                                $$ = node;
                                            }
         ;

METHOD_DECLS : METHOD_DECLS METHOD_DECL {
                                            Node* node = newNode_NonTerminal(N_METHOD_DECL, NULL, $1, $2, NULL);
                                            node->lineNo = yylineno;
                                            $$ = node;
                                        }
             | METHOD_DECL { $$ = $1; }
             ;

METHOD_PROFILE : Type T_ID T_OPENP {
                                                currentMethod = newSymbol(METH, $1, $2, 0);
                                                $$ = currentMethod;
                                            }
               | T_VOID T_ID T_OPENP {
                                                currentMethod = newSymbol(METH, TYPE_VOID, $2, 0);
                                                $$ = currentMethod;
                                            }
               ;

METHOD_DECL : METHOD_PROFILE PARAMS T_CLOSEP BLOCK_OR_EXTERN {
                                                                    Node* node = newNode_NonTerminal(N_METHOD_DECL, $1, $4, NULL, NULL);
                                                                    node->lineNo = yylineno;
                                                                    $$ = node;
                                                                    currentMethod = NULL;
                                                                }
            ;

PARAMS : /* vacio */    { $$ = NULL; }
       | PARAMS T_COMMA Type T_ID {
                                    newParameter($<symbol>0, $3, $4, 0);
                                    $$ = $1;
                                }
       | Type T_ID {
                        $$ = newParameter($<symbol>0, $1, $2, 0);
                    }
       ;

BLOCK_OR_EXTERN : BLOCK     { $$ = $1; }
                | T_EXTERN T_SEMIC {
                                        Node* node = newNode_NonTerminal(N_EXTERN, NULL, NULL, NULL, NULL);
                                        node->lineNo = yylineno;
                                        $$ = node;
                                    }
                ;

BLOCK : T_OPENB T_CLOSEB {
                            Node* node = newNode_NonTerminal(N_BLOCK, NULL, NULL, NULL, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
      | T_OPENB VAR_DECLS T_CLOSEB {
                                        Node* node = newNode_NonTerminal(N_BLOCK, NULL, $2, NULL, NULL);
                                        node->lineNo = yylineno;
                                        $$ = node;
                                    }
      | T_OPENB STATEMENTS T_CLOSEB {
                                        Node* node = newNode_NonTerminal(N_BLOCK, NULL, NULL, $2, NULL);
                                        node->lineNo = yylineno;
                                        $$ = node;
                                    }
      | T_OPENB VAR_DECLS STATEMENTS T_CLOSEB {
                                                Node* node = newNode_NonTerminal(N_BLOCK, NULL, $2, $3, NULL);
                                                node->lineNo = yylineno;
                                                $$ = node;
                                            }
      ;

Type : T_INTEGER    { $$ = TYPE_INTEGER; }
     | T_BOOL       { $$ = TYPE_BOOL; }
     ;

STATEMENTS : STATEMENTS STATEMENT {
                                    Node* node = newNode_NonTerminal(N_STATEMENT, NULL, $1, $2, NULL);
                                    node->lineNo = yylineno;
                                    $$ = node;
                                }
           | STATEMENT  { $$ = $1; }
           ;

STATEMENT : T_ID T_ASSIGN EXPR T_SEMIC {
                                            Symbol* v_sym = newSymbol(VAR, NON_TYPE, $1, 0);
                                            Node* left = newNode_Terminal(v_sym);
                                            left->lineNo = yylineno;

                                            Node* node = newNode_NonTerminal(N_ASSIGN, NULL, left, $3, NULL);
                                            node->lineNo = yylineno;
                                            $$ = node;
                                        }
          | METHOD_CALL T_SEMIC     { $$ = $1; }
          | T_IF T_OPENP EXPR T_CLOSEP T_THEN BLOCK ELSE_ST {
                                                                Node* thenNode = newNode_NonTerminal(N_THEN, NULL, $6, NULL, NULL);
                                                                thenNode->lineNo = yylineno;

                                                                Node* elseNode = newNode_NonTerminal(N_ELSE, NULL, $7, NULL, NULL);
                                                                elseNode->lineNo = yylineno;

                                                                Node* node = newNode_NonTerminal(N_IF, NULL, $3, thenNode, elseNode);
                                                                node->lineNo = yylineno;
                                                                $$ = node;
                                                            }
          | T_WHILE EXPR BLOCK {
                                    Node* node = newNode_NonTerminal(N_WHILE, NULL, $2, $3, NULL);
                                    node->lineNo = yylineno;
                                    $$ = node;
                                }
          | T_RETURN EXPR_ST T_SEMIC {
                                        Node* node = newNode_NonTerminal(N_RETURN, NULL, $2, NULL, NULL);
                                        node->lineNo = yylineno;
                                        $$ = node;
                                    }
          | T_SEMIC     { $$ = NULL; }
          | BLOCK       { $$ = $1; }
          ;

ELSE_ST : /* vacio */       { $$ = NULL; }
        | T_ELSE BLOCK      { $$ = $2; }
        ;

EXPR_ST : /* vacio */   { $$ = NULL; }
        | EXPR  { $$ = $1; }
        ;

METHOD_CALL : T_ID T_OPENP EXPRS T_CLOSEP {
                                            currentMethod = newSymbol(METH, NON_TYPE, $1, 0);

                                            Node* node = newNode_NonTerminal(N_METHOD_CALL, currentMethod, $3, NULL, NULL);
                                            node->lineNo = yylineno;
                                            $$ = node;
                                        }
            ;

EXPRS : /* vacio */     { $$ = NULL; }
      | EXPRS T_COMMA EXPR {
                                Node* node = newNode_NonTerminal(N_EXPR, NULL, $1, $3, NULL);
                                node->lineNo = yylineno;
                                $$ = node;
                            }
      | EXPR    {
                    $$ = $1;
                    currentMethod = NULL;
                }
      ;

EXPR : T_ID {
                Symbol* v_sym = newSymbol(VAR, NON_TYPE, $1, 0);

                Node* node = newNode_NonTerminal(N_EXPR, v_sym, NULL, NULL, NULL);
                node->lineNo = yylineno;
                $$ = node;
            }
     | METHOD_CALL      { $$ = $1; }
     | LITERAL          { $$ = $1; }
     | EXPR T_PLUS EXPR {
                            Node* node = newNode_NonTerminal(N_PLUS, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_MINUS EXPR {
                            Node* node = newNode_NonTerminal(N_MINUS, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_MULT EXPR {
                            Node* node = newNode_NonTerminal(N_MULT, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_DIVISION EXPR {
                                Node* node = newNode_NonTerminal(N_DIV, NULL, $1, $3, NULL);
                                node->lineNo = yylineno;
                                $$ = node;
                            }
     | EXPR T_MOD EXPR {
                            Node* node = newNode_NonTerminal(N_MOD, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_LESS EXPR {
                            Node* node = newNode_NonTerminal(N_LESS, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_GREATER EXPR {
                                Node* node = newNode_NonTerminal(N_GREAT, NULL, $1, $3, NULL);
                                node->lineNo = yylineno;
                                $$ = node;
                            }
     | EXPR T_EQUAL EXPR {
                            Node* node = newNode_NonTerminal(N_EQUAL, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_AND EXPR {
                            Node* node = newNode_NonTerminal(N_AND, NULL, $1, $3, NULL);
                            node->lineNo = yylineno;
                            $$ = node;
                        }
     | EXPR T_OR EXPR {
                        Node* node = newNode_NonTerminal(N_OR, NULL, $1, $3, NULL);
                        node->lineNo = yylineno;
                        $$ = node;
                    }
     | T_MINUS EXPR %prec U_MINUS {
                                    Node* node = newNode_NonTerminal(N_NEG, NULL, $2, NULL, NULL);
                                    node->lineNo = yylineno;
                                    $$ = node;
                                }
     | T_NOT EXPR {
                    Node* node = newNode_NonTerminal(N_NOT, NULL, $2, NULL, NULL);
                    node->lineNo = yylineno;
                    $$ = node;
                }
     | T_OPENP EXPR T_CLOSEP    { $$ = $2; }
     ;

LITERAL : INT_NUM {
                    Symbol* v_sym = newSymbol(CONST, TYPE_INTEGER, NULL, $1);

                    Node* node = newNode_Terminal(v_sym);
                    node->lineNo = yylineno;
                    $$ = node;
                }
        | T_TRUE {
                    Symbol* v_sym = newSymbol(CONST, TYPE_BOOL, NULL, 1);

                    Node* node = newNode_Terminal(v_sym);
                    node->lineNo = yylineno;
                    $$ = node;
                }
        | T_FALSE {
                    Symbol* v_sym = newSymbol(CONST, TYPE_BOOL, NULL, 0);

                    Node* node = newNode_Terminal(v_sym);
                    node->lineNo = yylineno;
                    $$ = node;
                }
        ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "ERROR en la linea %d: %s\n", yylineno, s);
}