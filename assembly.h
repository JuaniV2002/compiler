// assembly.h
#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>

typedef struct TacCode TacCode;

typedef struct VarInfo {
    char* name;
    int offset;
    struct VarInfo* next;
} VarInfo;

typedef struct ParamStack {
    char* values[6];
    int count;
} ParamStack;

typedef struct FuncContext {
    char* funcName;
    int stackSize;
    VarInfo* vars;
    ParamStack paramStack;
} FuncContext;

// Crea un nuevo contexto de funcion para generacion de codigo
FuncContext* newFuncContext(char* name);

// Agrega una variable al contexto y asigna su offset en el stack
void addVar(FuncContext* ctx, char* name);

// Obtiene el offset en el stack de una variable
int getVarOffset(FuncContext* ctx, char* name);

// Libera la memoria del contexto de funcion
void freeFuncContext(FuncContext* ctx);

// Genera el codigo ensamblador x86-64 a partir del TAC
void generateAssembly(TacCode* tac, FILE* output);

#endif // ASSEMBLY_H
