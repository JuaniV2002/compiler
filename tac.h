// tac.h
#ifndef TAC_H
#define TAC_H

#include "ast.h"

typedef enum {
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV, TAC_MOD,
    TAC_LT, TAC_GT, TAC_EQ,
    TAC_AND, TAC_OR, TAC_NOT, TAC_NEG,
    TAC_ASSIGN, TAC_COPY,
    TAC_LABEL, TAC_GOTO, TAC_IF_FALSE,
    TAC_PARAM, TAC_CALL, TAC_RETURN,
    TAC_BEGIN_FUNC, TAC_END_FUNC
} TacOp;

typedef struct TacInstr {
    TacOp op;
    char* dest;
    char* arg1;
    char* arg2;
    struct TacInstr* next;
} TacInstr;

typedef struct TacCode {
    TacInstr* head;
    TacInstr* tail;
    int tempCount;
    int labelCount;
} TacCode;

TacCode* initTAC();

char* newTemp(TacCode* tac);

char* newLabel(TacCode* tac);

void emitTAC(TacCode* tac, TacOp op, char* dest, char* arg1, char* arg2);

void generateTAC(Node* root, TacCode* tac);

void printTAC(TacCode* tac);

void freeTAC(TacCode* tac);

#endif // TAC_H
