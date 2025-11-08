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

// Inicializa la estructura de codigo TAC
TacCode* initTAC();

// Genera un nuevo nombre de temporal (t0, t1, ...)
char* newTemp(TacCode* tac);

// Genera un nuevo nombre de etiqueta (L0, L1, ...)
char* newLabel(TacCode* tac);

// Emite una nueva instruccion TAC a la lista
void emitTAC(TacCode* tac, TacOp op, char* dest, char* arg1, char* arg2);

// Genera el codigo TAC a partir del AST
void generateTAC(Node* root, TacCode* tac);

// Escribe el codigo TAC a un archivo de salida
void writeTAC(TacCode* tac, FILE* output);

// Imprime el codigo TAC por salida estandar
void printTAC(TacCode* tac);

// Libera toda la memoria del codigo TAC
void freeTAC(TacCode* tac);

#endif // TAC_H
