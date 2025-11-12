// optimizations.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbol.h"
#include "tac.h"
#include "optimizations.h"


ConstValue* newConstValue(char* varName, int value, int isConstant) {
    ConstValue* cv = (ConstValue*) malloc(sizeof(ConstValue));
    if (!cv) {
        fprintf(stderr, "Error al asignar memoria para ConstValue.\n");
        exit(EXIT_FAILURE);
    }
    cv->varName = varName ? strdup(varName) : NULL;
    cv->value = value;
    cv->isConstant = isConstant;
    cv->next = NULL;
    return cv;
}

void addConstValue(ConstValue** table, char* varName, int value, int isConstant) {
    if (!varName) return;
    
    // Buscar si ya existe y actualizar
    ConstValue* current = *table;
    while (current) {
        if (strcmp(current->varName, varName) == 0) {
            current->value = value;
            current->isConstant = isConstant;
            return;
        }
        current = current->next;
    }
    
    // Si no existe, agregar nuevo
    ConstValue* newCV = newConstValue(varName, value, isConstant);
    newCV->next = *table;
    *table = newCV;
}

ConstValue* findConstValue(ConstValue* table, char* varName) {
    if (!varName) return NULL;
    
    ConstValue* current = table;
    while (current) {
        if (strcmp(current->varName, varName) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void invalidateConstValue(ConstValue** table, char* varName) {
    if (!varName || !table) return;
    
    ConstValue* current = *table;
    while (current) {
        if (strcmp(current->varName, varName) == 0) {
            current->isConstant = 0;
            return;
        }
        current = current->next;
    }
}

void freeConstTable(ConstValue* table) {
    while (table) {
        ConstValue* temp = table;
        table = table->next;
        if (temp->varName) free(temp->varName);
        free(temp);
    }
}

// Estructura para rastrear uso de variables
typedef struct VarUsage {
    char* varName;
    int isUsed;         // 1 si la variable se lee en algún lugar
    int assignCount;    // Número de asignaciones
    struct VarUsage* next;
} VarUsage;

// Crea un nuevo registro de uso de variable
static VarUsage* newVarUsage(char* varName) {
    VarUsage* vu = (VarUsage*) malloc(sizeof(VarUsage));
    if (!vu) {
        fprintf(stderr, "Error al asignar memoria para VarUsage.\n");
        exit(EXIT_FAILURE);
    }
    vu->varName = varName ? strdup(varName) : NULL;
    vu->isUsed = 0;
    vu->assignCount = 0;
    vu->next = NULL;
    return vu;
}

// Busca o crea un registro de uso para una variable
static VarUsage* findOrCreateVarUsage(VarUsage** table, char* varName) {
    if (!varName) return NULL;
    
    // Buscar existente
    VarUsage* current = *table;
    while (current) {
        if (strcmp(current->varName, varName) == 0) {
            return current;
        }
        current = current->next;
    }
    
    // Crear nuevo
    VarUsage* newVU = newVarUsage(varName);
    newVU->next = *table;
    *table = newVU;
    return newVU;
}

// Libera la tabla de uso de variables
static void freeVarUsageTable(VarUsage* table) {
    while (table) {
        VarUsage* temp = table;
        table = table->next;
        if (temp->varName) free(temp->varName);
        free(temp);
    }
}

// Verifica si una cadena es un numero
static int isNumber(char* str) {
    if (!str || *str == '\0') return 0;
    
    int i = 0;
    if (str[0] == '-' || str[0] == '+') i++;
    
    if (str[i] == '\0') return 0;
    
    while (str[i] != '\0') {
        if (!isdigit(str[i])) return 0;
        i++;
    }
    return 1;
}

// Convierte una cadena a entero
static int stringToInt(char* str) {
    return atoi(str);
}

// Crea una cadena con un numero
static char* intToString(int value) {
    char* str = (char*) malloc(32);
    sprintf(str, "%d", value);
    return str;
}

// Verifica si una instrucción tiene efecto secundario (no puede eliminarse)
static int hasSideEffect(TacInstr* instr) {
    if (!instr) return 0;
    
    switch (instr->op) {
        case TAC_CALL:      // Llamadas a funciones
        case TAC_RETURN:    // Return
        case TAC_PARAM:     // Parámetros de llamada
        case TAC_LABEL:     // Etiquetas
        case TAC_GOTO:      // Saltos
        case TAC_IF_FALSE:  // Condicionales
        case TAC_BEGIN_FUNC:
        case TAC_END_FUNC:
            return 1;
        default:
            return 0;
    }
}

// Marca una variable como usada (leída)
static void markVariableAsUsed(VarUsage** table, char* varName) {
    if (!varName || isNumber(varName)) return;
    
    VarUsage* vu = findOrCreateVarUsage(table, varName);
    if (vu) {
        vu->isUsed = 1;
    }
}

// Marca una variable como asignada
static void markVariableAsAssigned(VarUsage** table, char* varName) {
    if (!varName || isNumber(varName)) return;
    
    VarUsage* vu = findOrCreateVarUsage(table, varName);
    if (vu) {
        vu->assignCount++;
    }
}

// Realiza constant folding para operaciones binarias
static int foldBinaryOp(TacOp op, int left, int right, int* result) {
    switch (op) {
        case TAC_ADD: *result = left + right; return 1;
        case TAC_SUB: *result = left - right; return 1;
        case TAC_MUL: *result = left * right; return 1;
        case TAC_DIV: 
            if (right == 0) return 0;  // Division por cero
            *result = left / right; 
            return 1;
        case TAC_MOD:
            if (right == 0) return 0;  // Division por cero
            *result = left % right;
            return 1;
        case TAC_LT: *result = (left < right) ? 1 : 0; return 1;
        case TAC_GT: *result = (left > right) ? 1 : 0; return 1;
        case TAC_EQ: *result = (left == right) ? 1 : 0; return 1;
        case TAC_AND: *result = (left && right) ? 1 : 0; return 1;
        case TAC_OR: *result = (left || right) ? 1 : 0; return 1;
        default: return 0;
    }
}

// Realiza constant folding para operaciones unarias
static int foldUnaryOp(TacOp op, int value, int* result) {
    switch (op) {
        case TAC_NOT: *result = !value; return 1;
        case TAC_NEG: *result = -value; return 1;
        default: return 0;
    }
}

int optimizeConstantPropagation(TacCode* tac) {
    if (!tac) return 0;
    
    int changesCount = 0;
    ConstValue* constTable = NULL;
    
    TacInstr* instr = tac->head;
    
    while (instr) {
        // Resetear tabla de constantes al entrar/salir de funciones
        if (instr->op == TAC_BEGIN_FUNC) {
            freeConstTable(constTable);
            constTable = NULL;
        }
        
        // PASO 1: Intentar reemplazar operandos con constantes conocidas
        if (instr->arg1 && !isNumber(instr->arg1)) {
            ConstValue* cv = findConstValue(constTable, instr->arg1);
            if (cv && cv->isConstant) {
                free(instr->arg1);
                instr->arg1 = intToString(cv->value);
                changesCount++;
            }
        }
        
        if (instr->arg2 && !isNumber(instr->arg2)) {
            ConstValue* cv = findConstValue(constTable, instr->arg2);
            if (cv && cv->isConstant) {
                free(instr->arg2);
                instr->arg2 = intToString(cv->value);
                changesCount++;
            }
        }
        
        // PASO 2: Realizar constant folding y actualizar tabla
        switch (instr->op) {
            case TAC_COPY:
                // x = constante o x = y (donde y es constante)
                if (isNumber(instr->arg1)) {
                    int value = stringToInt(instr->arg1);
                    addConstValue(&constTable, instr->dest, value, 1);
                } else {
                    ConstValue* cv = findConstValue(constTable, instr->arg1);
                    if (cv && cv->isConstant) {
                        addConstValue(&constTable, instr->dest, cv->value, 1);
                    } else {
                        invalidateConstValue(&constTable, instr->dest);
                    }
                }
                break;
                
            case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV: case TAC_MOD:
            case TAC_LT: case TAC_GT: case TAC_EQ: case TAC_AND: case TAC_OR:
                // Si ambos operandos son constantes, plegar la operacion
                if (isNumber(instr->arg1) && isNumber(instr->arg2)) {
                    int left = stringToInt(instr->arg1);
                    int right = stringToInt(instr->arg2);
                    int result;
                    
                    if (foldBinaryOp(instr->op, left, right, &result)) {
                        // Convertir a operacion COPY
                        instr->op = TAC_COPY;
                        free(instr->arg1);
                        instr->arg1 = intToString(result);
                        if (instr->arg2) {
                            free(instr->arg2);
                            instr->arg2 = NULL;
                        }
                        
                        // Actualizar tabla
                        addConstValue(&constTable, instr->dest, result, 1);
                        changesCount++;
                    } else {
                        invalidateConstValue(&constTable, instr->dest);
                    }
                } else {
                    invalidateConstValue(&constTable, instr->dest);
                }
                break;
                
            case TAC_NOT: case TAC_NEG:
                // Si el operando es constante, plegar la operacion
                if (isNumber(instr->arg1)) {
                    int value = stringToInt(instr->arg1);
                    int result;
                    
                    if (foldUnaryOp(instr->op, value, &result)) {
                        // Convertir a operacion COPY
                        instr->op = TAC_COPY;
                        free(instr->arg1);
                        instr->arg1 = intToString(result);
                        
                        // Actualizar tabla
                        addConstValue(&constTable, instr->dest, result, 1);
                        changesCount++;
                    } else {
                        invalidateConstValue(&constTable, instr->dest);
                    }
                } else {
                    invalidateConstValue(&constTable, instr->dest);
                }
                break;
                
            case TAC_CALL:
                // Las llamadas a funciones invalidan el destino
                invalidateConstValue(&constTable, instr->dest);
                break;
                
            case TAC_END_FUNC:
                // Limpiar tabla al salir de funcion
                freeConstTable(constTable);
                constTable = NULL;
                break;
                
            default:
                break;
        }
        
        instr = instr->next;
    }
    
    freeConstTable(constTable);
    
    if (changesCount > 0) {
        printf("Optimizacion: Propagacion de constantes aplicada (%d cambios)\n", changesCount);
    }
    
    return changesCount;
}

int optimizeDeadCode(TacCode* tac) {
    if (!tac) return 0;
    
    int changesCount = 0;
    VarUsage* usageTable = NULL;
    
    // PASO 1: Analizar uso de variables (dos pasadas)
    // Primera pasada: marcar todas las asignaciones
    TacInstr* instr = tac->head;
    while (instr) {
        // Marcar destinos como asignados
        if (instr->dest && !isNumber(instr->dest)) {
            switch (instr->op) {
                case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV: case TAC_MOD:
                case TAC_LT: case TAC_GT: case TAC_EQ:
                case TAC_AND: case TAC_OR: case TAC_NOT: case TAC_NEG:
                case TAC_COPY: case TAC_CALL:
                    markVariableAsAssigned(&usageTable, instr->dest);
                    break;
                default:
                    break;
            }
        }
        instr = instr->next;
    }
    
    // Segunda pasada: marcar variables usadas (leídas)
    instr = tac->head;
    while (instr) {
        // Marcar arg1 como usado
        if (instr->arg1 && !isNumber(instr->arg1)) {
            switch (instr->op) {
                case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV: case TAC_MOD:
                case TAC_LT: case TAC_GT: case TAC_EQ:
                case TAC_AND: case TAC_OR: case TAC_NOT: case TAC_NEG:
                case TAC_COPY: case TAC_RETURN: case TAC_IF_FALSE: case TAC_PARAM:
                    markVariableAsUsed(&usageTable, instr->arg1);
                    break;
                default:
                    break;
            }
        }
        
        // Marcar arg2 como usado
        if (instr->arg2 && !isNumber(instr->arg2)) {
            switch (instr->op) {
                case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV: case TAC_MOD:
                case TAC_LT: case TAC_GT: case TAC_EQ:
                case TAC_AND: case TAC_OR: case TAC_IF_FALSE:
                    markVariableAsUsed(&usageTable, instr->arg2);
                    break;
                default:
                    break;
            }
        }
        
        instr = instr->next;
    }
    
    // PASO 2: Eliminar asignaciones a variables no usadas
    TacInstr* prev = NULL;
    instr = tac->head;
    
    while (instr) {
        int shouldRemove = 0;
        
        // Solo considerar eliminar si no tiene efectos secundarios
        if (!hasSideEffect(instr) && instr->dest) {
            VarUsage* vu = findOrCreateVarUsage(&usageTable, instr->dest);
            
            // Eliminar si la variable nunca se usa
            if (vu && !vu->isUsed) {
                shouldRemove = 1;
            }
        }
        
        if (shouldRemove) {
            TacInstr* toRemove = instr;
            
            // Actualizar punteros
            if (prev) {
                prev->next = instr->next;
            } else {
                tac->head = instr->next;
            }
            
            // Si era el último, actualizar tail
            if (instr == tac->tail) {
                tac->tail = prev;
            }
            
            instr = instr->next;
            
            // Liberar la instrucción eliminada
            if (toRemove->dest) free(toRemove->dest);
            if (toRemove->arg1) free(toRemove->arg1);
            if (toRemove->arg2) free(toRemove->arg2);
            free(toRemove);
            
            changesCount++;
        } else {
            prev = instr;
            instr = instr->next;
        }
    }
    
    // PASO 3: Eliminar código inalcanzable después de return/goto
    prev = NULL;
    instr = tac->head;
    int inUnreachableCode = 0;
    
    while (instr) {
        // Detectar inicio de código inalcanzable
        if (instr->op == TAC_RETURN || instr->op == TAC_GOTO) {
            inUnreachableCode = 1;
            prev = instr;
            instr = instr->next;
            continue;
        }
        
        // Resetear al encontrar una etiqueta o función
        if (instr->op == TAC_LABEL || instr->op == TAC_BEGIN_FUNC || instr->op == TAC_END_FUNC) {
            inUnreachableCode = 0;
            prev = instr;
            instr = instr->next;
            continue;
        }
        
        // Eliminar si estamos en código inalcanzable
        if (inUnreachableCode) {
            TacInstr* toRemove = instr;
            
            if (prev) {
                prev->next = instr->next;
            } else {
                tac->head = instr->next;
            }
            
            if (instr == tac->tail) {
                tac->tail = prev;
            }
            
            instr = instr->next;
            
            if (toRemove->dest) free(toRemove->dest);
            if (toRemove->arg1) free(toRemove->arg1);
            if (toRemove->arg2) free(toRemove->arg2);
            free(toRemove);
            
            changesCount++;
        } else {
            prev = instr;
            instr = instr->next;
        }
    }
    
    freeVarUsageTable(usageTable);
    
    if (changesCount > 0) {
        printf("Optimización: Eliminación de código muerto aplicada (%d instrucciones eliminadas)\n", changesCount);
    }
    
    return changesCount;
}

int optimizeShifts(TacCode *tac) {
    if (!tac) return 0;
    
    int changesCount = 0;
    TacInstr* instr = tac->head;
    
    while (instr) {
        // Solo procesar multiplicaciones y divisiones
        if (instr->op == TAC_MUL || instr->op == TAC_DIV) {
            int value = 0;
            int isArg1Const = 0;
            int isArg2Const = 0;
            
            // Verificar si arg2 es un número (constante)
            if (instr->arg2 && isNumber(instr->arg2)) {
                value = stringToInt(instr->arg2);
                isArg2Const = 1;
            }
            // Para multiplicación, también verificar si arg1 es constante (conmutativa)
            else if (instr->op == TAC_MUL && instr->arg1 && isNumber(instr->arg1)) {
                value = stringToInt(instr->arg1);
                isArg1Const = 1;
            }
            
            // Verificar si el valor es una potencia de 2 positiva
            if ((isArg1Const || isArg2Const) && value > 0 && (value & (value - 1)) == 0) {
                // Calcular el exponente (número de bits a desplazar)
                int shiftAmount = 0;
                int temp = value;
                while (temp > 1) {
                    temp >>= 1;
                    shiftAmount++;
                }
                
                // Convertir la operación
                if (instr->op == TAC_MUL) {
                    // x * 2^n  ->  x << n  o  2^n * x  ->  x << n
                    instr->op = TAC_SHL;
                    
                    // Si la constante estaba en arg1, mover arg2 a arg1
                    if (isArg1Const) {
                        free(instr->arg1);
                        instr->arg1 = instr->arg2;
                        instr->arg2 = intToString(shiftAmount);
                    } else {
                        // Reemplazar arg2 con el exponente
                        free(instr->arg2);
                        instr->arg2 = intToString(shiftAmount);
                    }
                } else {
                    // x / 2^n  ->  x >> n
                    // (División no es conmutativa, solo arg2 puede ser constante)
                    instr->op = TAC_SHR;
                    free(instr->arg2);
                    instr->arg2 = intToString(shiftAmount);
                }
                
                changesCount++;
            }
        }
        
        instr = instr->next;
    }
    
    if (changesCount > 0) {
        printf("Optimización: Conversión a shifts aplicada (%d cambios)\n", changesCount);
    }
    
    return changesCount;
}

int optimizeAlgebraicIdentities(TacCode *tac) {
    if (!tac) return 0;
    
    int changesCount = 0;
    TacInstr* instr = tac->head;
    
    while (instr) {
        int simplified = 0;
        
        switch (instr->op) {
            case TAC_ADD:
                // x + 0 → x  o  0 + x → x
                if (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) == 0) {
                    // x + 0 → x
                    instr->op = TAC_COPY;
                    free(instr->arg2);
                    instr->arg2 = NULL;
                    simplified = 1;
                } else if (instr->arg1 && isNumber(instr->arg1) && stringToInt(instr->arg1) == 0) {
                    // 0 + x → x
                    instr->op = TAC_COPY;
                    free(instr->arg1);
                    instr->arg1 = instr->arg2;
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                break;
                
            case TAC_SUB:
                // x - 0 → x
                if (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) == 0) {
                    instr->op = TAC_COPY;
                    free(instr->arg2);
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                // 0 - x → -x
                else if (instr->arg1 && isNumber(instr->arg1) && stringToInt(instr->arg1) == 0) {
                    instr->op = TAC_NEG;
                    free(instr->arg1);
                    instr->arg1 = instr->arg2;
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                break;
                
            case TAC_MUL:
                // x * 0 → 0  o  0 * x → 0
                if ((instr->arg1 && isNumber(instr->arg1) && stringToInt(instr->arg1) == 0) ||
                    (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) == 0)) {
                    instr->op = TAC_COPY;
                    if (instr->arg1) free(instr->arg1);
                    if (instr->arg2) free(instr->arg2);
                    instr->arg1 = intToString(0);
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                // x * 1 → x
                else if (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) == 1) {
                    instr->op = TAC_COPY;
                    free(instr->arg2);
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                // 1 * x → x
                else if (instr->arg1 && isNumber(instr->arg1) && stringToInt(instr->arg1) == 1) {
                    instr->op = TAC_COPY;
                    free(instr->arg1);
                    instr->arg1 = instr->arg2;
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                break;
                
            case TAC_DIV:
                // 0 / x → 0 (solo si x es una constante != 0)
                if (instr->arg1 && isNumber(instr->arg1) && stringToInt(instr->arg1) == 0) {
                    // Solo optimizar si arg2 es una constante y != 0
                    if (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) != 0) {
                        instr->op = TAC_COPY;
                        free(instr->arg2);
                        instr->arg2 = NULL;
                        simplified = 1;
                    }
                    // Si arg2 no es constante o es 0, no optimizar (mantener para detectar error en runtime)
                }
                // x / 1 → x
                else if (instr->arg2 && isNumber(instr->arg2) && stringToInt(instr->arg2) == 1) {
                    instr->op = TAC_COPY;
                    free(instr->arg2);
                    instr->arg2 = NULL;
                    simplified = 1;
                }
                break;
                
            default:
                break;
        }
        
        if (simplified) {
            changesCount++;
        }
        
        instr = instr->next;
    }
    
    if (changesCount > 0) {
        printf("Optimización: Identidades algebraicas aplicadas (%d cambios)\n", changesCount);
    }
    
    return changesCount;
}

int applyOptimizations(TacCode* tac, char* optName) {
    if (!tac || !optName) return 0;
    
    int totalChanges = 0;
    
    if (strcmp(optName, "all") == 0) {
        // Aplicar todas las optimizaciones
        totalChanges += optimizeAlgebraicIdentities(tac);
        totalChanges += optimizeConstantPropagation(tac);
        totalChanges += optimizeDeadCode(tac);
        totalChanges += optimizeShifts(tac);
    }
    else if (strcmp(optName, "constant-propagation") == 0) {
        totalChanges += optimizeConstantPropagation(tac);
    }
    else if (strcmp(optName, "dead-code") == 0) {
        totalChanges += optimizeDeadCode(tac);
    }
    else if (strcmp(optName, "pattern-matching") == 0) {
        totalChanges += optimizeShifts(tac);
        totalChanges += optimizeAlgebraicIdentities(tac);
    }
    else {
        fprintf(stderr, "Advertencia: Optimizacion desconocida '%s'\n", optName);
        return 0;
    }
    
    return totalChanges;
}
