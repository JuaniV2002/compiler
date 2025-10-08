#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ts.h"

// Inicializa la tabla de simbolos vacia con un nivel 0
Level* initializeTS() {
    Level* levelNode = (Level*) malloc(sizeof(Level));
    if (!levelNode) {
        fprintf(stderr, "Error al asignar memoria para la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }
    levelNode->tsNode = NULL;

    levelNode->levelNumber = 0;
    levelNode->nextLevel = NULL;
    return levelNode;
}

// Retorna el nivel mas alto de la tabla de simbolos (tope de la pila)
Level* topLevel(Level* firstLevel) {
    Level* current = firstLevel;
    while (current->nextLevel) {
        current = current->nextLevel;
    }
    return current;
}

// Retorna el nivel anterior a un nivel dado de la tabla de simbolos
Level* findLevel(Level* firstLevel, Level* NextToTargetLevel) {
    Level* current = firstLevel;
    while (current->nextLevel && current->nextLevel != NextToTargetLevel) {
        current = current->nextLevel;
    }
    return current;
}

// Abre un nuevo nivel en la tabla de simbolos y lo enlaza al final de la lista encadenada (funciona como una pila con el tope al final)
Level* openNewLevel(Level* firstLevel) {
    Level* newLevel = (Level*) malloc(sizeof(Level));
    if (!newLevel) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nivel de la tabla de simbolos.\n");
        exit(EXIT_FAILURE);
    }

    Level* top = topLevel(firstLevel);

    newLevel->tsNode = NULL;
    newLevel->levelNumber = top->levelNumber + 1;
    newLevel->nextLevel = NULL;

    top->nextLevel = newLevel;

    return newLevel;
}

// Cierra el nivel mas alto (tope de la pila) de la tabla de simbolos y libera su memoria
void closeLevel(Level* firstLevel) {
    if (!firstLevel) return;

    Level* top = topLevel(firstLevel);
    Level* beforeTop = findLevel(firstLevel, top);

    TSNode* currentTSNode = top->tsNode;
    while (currentTSNode) {
        TSNode* temp = currentTSNode;
        currentTSNode = currentTSNode->next;
        
        free(temp);     // Liberar el nodo de la tabla
    }
    beforeTop->nextLevel = NULL;
    free(top);
}

// Retorna el ultimo nodo de un nivel dado
TSNode* lastTSNode(Level* symbolTable) {
    if (!symbolTable || !symbolTable->tsNode) return NULL;

    TSNode* current = symbolTable->tsNode;
    while (current->next) {
        current = current->next;
    }
    return current;
}

// Inserta un nuevo simbolo al final del nivel mas alto de la tabla de simbolos (tope de la pila)
Symbol* insertSymbol(Level* symbolTable, flagType flag, infoType type, char* name, int value) {
    if (getSymbol(symbolTable, name)) {
        fprintf(stderr, "Error: El simbolo '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    } 

    Symbol* newSym = newSymbol(flag, type, name, value);

    TSNode* newTSNode = (TSNode*) malloc(sizeof(TSNode));
    newTSNode->symbol = newSym;
    newTSNode->next = NULL;

    TSNode* last = lastTSNode(symbolTable);
    if (!last) {
        symbolTable->tsNode = newTSNode;
        return newSym;
    }

    last->next = newTSNode;
    return newSym;
}

// Agrega un nuevo parametro al final de la lista de parametros de un metodo dado
Symbol* insertParameter(Symbol* method, infoType type, char* name, int value) {
    if (getParameter(method, name)) {
        fprintf(stderr, "Error: El parametro '%s' ya existe en la tabla de simbolos.\n", name);
        return NULL;
    }

    Symbol* newParam = newParameter(method, type, name, value);
    if (!newParam) {
        return NULL;
    }

    return newParam;
}

// Busca un simbolo por nombre en un nivel dado de la tabla de simbolos
Symbol* getSymbol(Level* symbolTable, char* name) {
    TSNode* current = symbolTable->tsNode;

    while (current) {
        // Si el simbolo es un metodo, tambien buscar en su lista de parametros
        if (current->symbol && current->symbol->flag == METH) {
            Symbol* param = current->symbol->nextParam;
            while (param) {
                if (param && strcmp(param->name, name) == 0) {
                    return param;
                }
                param = param->nextParam;
            }
        }
        
        // Si el simbolo coincide, retornarlo
        if (current->symbol && strcmp(current->symbol->name, name) == 0) {
            return current->symbol;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

// Libera solo la estructura de la tabla de simbolos (niveles y TSNodes), NO los símbolos
void freeTS(Level* symbolTable) {
    Level* currentLevel = symbolTable;
    while (currentLevel) {
        Level* tempLevel = currentLevel;
        currentLevel = currentLevel->nextLevel;
        
        // Liberar solo los TSNodes, NO los símbolos (quedan vinculados al AST)
        TSNode* currentTSNode = tempLevel->tsNode;
        while (currentTSNode) {
            TSNode* temp = currentTSNode;
            currentTSNode = currentTSNode->next;
            free(temp);  // Solo libera el nodo de la tabla, NO el símbolo
        }
        
        // Liberar el nivel
        free(tempLevel);
    }
}

// Función auxiliar para obtener el tipo como string
const char* getTypeString(infoType type) {
    switch (type) {
        case TYPE_VOID: return "void";
        case TYPE_INTEGER: return "integer";
        case TYPE_BOOL: return "bool";
        default: return "unknown";
    }
}

// Función auxiliar para obtener el flag como string
const char* getFlagString(flagType flag) {
    switch (flag) {
        case METH: return "METH";
        case VAR: return "VAR";
        case PARAMET: return "PARAMET";
        case CONST: return "CONST";
        default: return "UNKNOWN";
    }
}

// Estructura para almacenar las líneas de información de cada símbolo
#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int lineCount;
    int columnWidth;
} SymbolInfo;

// Obtiene el ancho necesario para la columna de un símbolo (sin códigos de color)
int getSymbolColumnWidth(Symbol* sym) {
    if (!sym) return 0;
    
    int maxWidth = strlen("Flag: ") + strlen(getFlagString(sym->flag));
    
    if (sym->name) {
        int nameWidth = strlen("Nombre: ''") + strlen(sym->name);
        if (nameWidth > maxWidth) maxWidth = nameWidth;
    }
    
    int typeWidth = strlen("Tipo: ") + strlen(getTypeString(sym->type));
    if (typeWidth > maxWidth) maxWidth = typeWidth;
    
    if ((sym->flag == VAR || sym->flag == CONST || sym->flag == PARAMET) && 
        (sym->type == TYPE_INTEGER || sym->type == TYPE_BOOL)) {
        char valueStr[32];
        sprintf(valueStr, "%d", sym->value);
        int valueWidth = strlen("Valor: ") + strlen(valueStr);
        if (valueWidth > maxWidth) maxWidth = valueWidth;
    }
    
    return maxWidth;
}

// Cuenta las líneas que ocupará un símbolo (incluyendo parámetros)
int countSymbolLines(Symbol* sym) {
    if (!sym) return 0;
    
    int lines = 0;
    
    // Flag
    lines++;
    
    // Nombre (si existe)
    if (sym->name) lines++;
    
    // Tipo
    lines++;
    
    // Valor (si aplica)
    if ((sym->flag == VAR || sym->flag == CONST || sym->flag == PARAMET) && 
        (sym->type == TYPE_INTEGER || sym->type == TYPE_BOOL)) {
        lines++;
    }
    
    // Parámetros (si es un método con parámetros)
    if (sym->flag == METH && sym->nextParam) {
        lines++; // "Parametros:"
        lines++; // "|"
        lines++; // "v"
        
        Symbol* param = sym->nextParam;
        while (param) {
            lines++; // "Parametro"
            lines++; // Flag
            lines++; // Nombre
            lines++; // Tipo
            if (param->type == TYPE_INTEGER || param->type == TYPE_BOOL) {
                lines++; // Valor
            }
            lines++; // "|"
            lines++; // "v"
            
            param = param->nextParam;
        }
        lines++; // "NULL"
    }
    
    return lines;
}

// Genera las líneas de información de un símbolo
void generateSymbolInfo(Symbol* sym, SymbolInfo* info) {
    if (!sym || !info) return;
    
    info->lineCount = 0;
    info->columnWidth = getSymbolColumnWidth(sym);
    
    // Determinar el color según el tipo de símbolo
    const char* color;
    switch (sym->flag) {
        case METH: color = "\033[1;34m"; break;  // Azul para métodos
        case VAR: color = "\033[1;32m"; break;    // Verde para variables
        case PARAMET: color = "\033[1;35m"; break; // Magenta para parámetros
        case CONST: color = "\033[1;33m"; break;   // Amarillo para constantes
        default: color = "\033[1;37m"; break;
    }
    
    // Flag
    snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
             "Flag: %s%s\033[0m", color, getFlagString(sym->flag));
    
    // Nombre (si existe)
    if (sym->name) {
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                 "Nombre: '%s%s\033[0m'", color, sym->name);
    }
    
    // Tipo
    snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
             "Tipo: %s%s\033[0m", color, getTypeString(sym->type));
    
    // Valor (si aplica)
    if ((sym->flag == VAR || sym->flag == CONST || sym->flag == PARAMET) && 
        (sym->type == TYPE_INTEGER || sym->type == TYPE_BOOL)) {
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                 "Valor: %s%d\033[0m", color, sym->value);
    }
    
    // Parámetros (si es un método con parámetros)
    if (sym->flag == METH && sym->nextParam) {
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                 "\033[1;35mParametros:\033[0m");
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, "     |");
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, "     v");
        
        Symbol* param = sym->nextParam;
        while (param) {
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                     "     \033[1;35mParametro\033[0m");
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                     "     Flag: \033[1;35m%s\033[0m", getFlagString(param->flag));
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                     "     Nombre: '\033[1;35m%s\033[0m'", param->name ? param->name : "unnamed");
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                     "     Tipo: \033[1;35m%s\033[0m", getTypeString(param->type));
            
            if (param->type == TYPE_INTEGER || param->type == TYPE_BOOL) {
                snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                         "     Valor: \033[1;35m%d\033[0m", param->value);
            }
            
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, "     |");
            snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, "     v");
            
            param = param->nextParam;
        }
        snprintf(info->lines[info->lineCount++], MAX_LINE_LENGTH, 
                 "     \033[1;90mNULL\033[0m");
    }
}

// Función para obtener el largo visual de una cadena (sin códigos ANSI)
int getVisualLength(const char* str) {
    int length = 0;
    int inEscape = 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\033') {
            inEscape = 1;
        } else if (inEscape && str[i] == 'm') {
            inEscape = 0;
        } else if (!inEscape) {
            length++;
        }
    }
    
    return length;
}

// Imprime la tabla de simbolos completa con formato horizontal
void printTS(Level* symbolTable) {
    if (!symbolTable) {
        printf("\033[1;31m(Empty Symbol Table)\033[0m\n");
        return;
    }
    
    printf("\n\033[1;36m=== SYMBOL TABLE (Stack of Levels) ===\033[0m\n\n");
    
    // Recorrer niveles desde el nivel 0 hasta el más alto
    Level* currentLevel = symbolTable;
    
    while (currentLevel) {
        // Contar símbolos en el nivel
        TSNode* currentNode = currentLevel->tsNode;
        int symbolCount = 0;
        while (currentNode) {
            symbolCount++;
            currentNode = currentNode->next;
        }
        
        if (symbolCount == 0) {
            // Imprimir encabezado del nivel
            printf("\033[1;36mNivel %d\033[0m -> ", currentLevel->levelNumber);
            printf("\033[1;90mNULL\033[0m\n");
        } else {
            // Generar información de cada símbolo primero para calcular anchos
            SymbolInfo symbolInfos[100]; // Máximo 100 símbolos por nivel
            int maxLines = 0;
            
            currentNode = currentLevel->tsNode;
            for (int i = 0; i < symbolCount && currentNode; i++) {
                generateSymbolInfo(currentNode->symbol, &symbolInfos[i]);
                if (symbolInfos[i].lineCount > maxLines) {
                    maxLines = symbolInfos[i].lineCount;
                }
                currentNode = currentNode->next;
            }
            
            // Calcular posiciones de columna basadas en ancho máximo de cada símbolo
            int columnPositions[100];
            int columnWidths[100];
            
            for (int i = 0; i < symbolCount; i++) {
                // Asegurar que el ancho sea suficiente para "Simbolo" (7 chars)
                int minWidth = 7; // longitud de "Simbolo"
                columnWidths[i] = (symbolInfos[i].columnWidth > minWidth ? symbolInfos[i].columnWidth : minWidth) + 6; // +6 para más espaciado
            }
            
            // Agregar margen inicial a todas las columnas
            int leftMargin = 12; // Espacios después de "  |"
            
            columnPositions[0] = leftMargin;
            for (int i = 1; i < symbolCount; i++) {
                columnPositions[i] = columnPositions[i-1] + columnWidths[i-1];
            }
            
            // Imprimir encabezado del nivel con símbolos alineados
            printf("\033[1;36mNivel %d\033[0m ", currentLevel->levelNumber);
            
            // Agregar guiones hasta el primer símbolo
            for (int d = 0; d < columnPositions[0] - 2; d++) { // -2 para "> "
                printf("-");
            }
            printf("> ");
            
            int currentHeaderPos = columnPositions[0];
            
            for (int i = 0; i < symbolCount; i++) {
                // Si no es el primer símbolo, calcular espacios hasta la columna
                if (i > 0) {
                    int spacesToAdd = columnPositions[i] - currentHeaderPos;
                    for (int sp = 0; sp < spacesToAdd; sp++) {
                        printf(" ");
                    }
                    currentHeaderPos += spacesToAdd;
                }
                
                // Imprimir "Simbolo"
                printf("\033[1;34mSimbolo\033[0m");
                currentHeaderPos += 7; // longitud de "Simbolo"
                
                // Si no es el último símbolo
                if (i < symbolCount - 1) {
                    // Calcular cuántos guiones necesitamos hasta la siguiente columna
                    int spaceUntilNext = columnPositions[i+1] - currentHeaderPos;
                    printf(" ");
                    currentHeaderPos += 1;
                    
                    for (int d = 0; d < spaceUntilNext - 3; d++) { // -3 para "> "
                        printf("-");
                    }
                    currentHeaderPos += spaceUntilNext - 3;
                    
                    printf("> ");
                    currentHeaderPos += 2;
                } else {
                    // Para el último símbolo
                    printf(" ");
                    for (int d = 0; d < 3; d++) {
                        printf("-");
                    }
                    printf("> \033[1;90mNULL\033[0m");
                }
            }
            printf("\n");
            
            // Imprimir línea por línea
            for (int line = 0; line < maxLines; line++) {
                printf("  |");
                
                int currentPos = 0; // Posición actual en la línea
                
                for (int sym = 0; sym < symbolCount; sym++) {
                    // Calcular espacios necesarios para llegar a la columna del símbolo
                    int spacesToAdd = columnPositions[sym] - currentPos;
                    
                    // Imprimir espacios
                    for (int sp = 0; sp < spacesToAdd; sp++) {
                        printf(" ");
                    }
                    currentPos += spacesToAdd;
                    
                    // Imprimir contenido o espacios vacíos
                    if (line < symbolInfos[sym].lineCount) {
                        printf("%s", symbolInfos[sym].lines[line]);
                        int lineLength = getVisualLength(symbolInfos[sym].lines[line]);
                        currentPos += lineLength;
                        
                        // Rellenar con espacios hasta completar el ancho de la columna
                        int paddingNeeded = columnWidths[sym] - lineLength;
                        for (int sp = 0; sp < paddingNeeded; sp++) {
                            printf(" ");
                        }
                        currentPos += paddingNeeded;
                    } else {
                        // Imprimir espacios vacíos para mantener alineación
                        for (int sp = 0; sp < columnWidths[sym]; sp++) {
                            printf(" ");
                        }
                        currentPos += columnWidths[sym];
                    }
                }
                
                printf("\n");
            }
        }
        
        // Imprimir flecha hacia el siguiente nivel
        printf("  |\n");
        printf("  v\n");
        
        currentLevel = currentLevel->nextLevel;
    }
    
    // Imprimir NULL al final de la pila
    printf("\033[1;90mNULL\033[0m\n");
    
    printf("\n\033[1;36m======================================\033[0m\n\n");
}