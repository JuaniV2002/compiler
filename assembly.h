// assembly.h
// Modulo de generacion de codigo ensamblador x86-64
//
// Este modulo toma el codigo de tres direcciones (TAC) y genera
// codigo ensamblador x86-64 simple pero correcto. Sigue la convencion
// de llamada de Linux x86-64 ABI.
//
// Caracteristicas:
// - Todas las variables y temporales residen en el stack frame
// - Usa %r10 y %r11 como registros temporales para expresiones
// - Soporta hasta 6 parametros en registros (%rdi, %rsi, %rdx, %rcx, %r8, %r9)
// - Genera prologos/epilogos con enter/leave/ret
// - Maneja operaciones aritmeticas, logicas y de control de flujo
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

FuncContext* newFuncContext(char* name);

void addVar(FuncContext* ctx, char* name);

int getVarOffset(FuncContext* ctx, char* name);

void freeFuncContext(FuncContext* ctx);

void generateAssembly(TacCode* tac, FILE* output);

#endif // ASSEMBLY_H
