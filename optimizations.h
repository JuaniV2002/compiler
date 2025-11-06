// optimizations.h
// Modulo de optimizaciones sobre codigo de tres direcciones (TAC)
//
// Este modulo implementa diferentes tecnicas de optimizacion que se aplican
// sobre el TAC antes de generar codigo ensamblador. Las optimizaciones
// mejoran el rendimiento y reducen el tamaño del codigo generado.
//
// Optimizaciones soportadas:
// - constant-propagation: Propaga constantes y elimina calculos redundantes
// - dead-code: Elimina codigo muerto (no implementado aun)
// - all: Aplica todas las optimizaciones disponibles

#ifndef OPTIMIZATIONS_H
#define OPTIMIZATIONS_H

#include "tac.h"

// Estructura para rastrear valores constantes de variables
typedef struct ConstValue {
    char* varName;
    int value;
    int isConstant;  // 1 si tiene valor constante conocido, 0 si no
    struct ConstValue* next;
} ConstValue;

// Aplica las optimizaciones especificadas al codigo TAC
// optName puede ser: "constant-propagation", "dead-code", "all", etc.
// Retorna 1 si se aplicaron optimizaciones, 0 si no
int applyOptimizations(TacCode* tac, char* optName);

// Optimizacion: Propagacion de constantes
// Reemplaza variables con valores constantes conocidos
// y realiza constant folding (plegar constantes)
int optimizeConstantPropagation(TacCode* tac);

// Optimizacion: Eliminacion de codigo muerto (placeholder)
int optimizeDeadCode(TacCode* tac);

// Funciones auxiliares para manejo de tabla de constantes
ConstValue* newConstValue(char* varName, int value, int isConstant);
void addConstValue(ConstValue** table, char* varName, int value, int isConstant);
ConstValue* findConstValue(ConstValue* table, char* varName);
void invalidateConstValue(ConstValue** table, char* varName);
void freeConstTable(ConstValue* table);

#endif // OPTIMIZATIONS_H
