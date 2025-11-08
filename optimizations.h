// optimizations.h
#ifndef OPTIMIZATIONS_H
#define OPTIMIZATIONS_H

#include "tac.h"

typedef struct ConstValue {
    char* varName;
    int value;
    int isConstant;  // 1 si tiene valor constante conocido, 0 si no
    struct ConstValue* next;
} ConstValue;

// Aplica las optimizaciones especificadas al codigo TAC
int applyOptimizations(TacCode* tac, char* optName);

// Propaga constantes y realiza constant folding
int optimizeConstantPropagation(TacCode* tac);

// Elimina codigo muerto no alcanzable
int optimizeDeadCode(TacCode* tac);

// Crea un nuevo registro de valor constante
ConstValue* newConstValue(char* varName, int value, int isConstant);

// Agrega o actualiza un valor constante en la tabla
void addConstValue(ConstValue** table, char* varName, int value, int isConstant);

// Busca un valor constante en la tabla
ConstValue* findConstValue(ConstValue* table, char* varName);

// Marca una variable como no constante
void invalidateConstValue(ConstValue** table, char* varName);

// Libera la memoria de la tabla de constantes
void freeConstTable(ConstValue* table);

#endif // OPTIMIZATIONS_H
