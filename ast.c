#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"

Node* newNode_Terminal(Symbol* symbol) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = N_TERM;

    newNode->sym = symbol;
    if (!newNode->sym) {
        fprintf(stderr, "Error al crear símbolo para nodo terminal.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    newNode->lineNo = 0;

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->third = NULL;

    return newNode;
}

Node* newNode_NonTerminal(nodeType type, Symbol* symbol, struct Node* left, struct Node* right, struct Node* third) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error al asignar memoria para un nuevo nodo no terminal.\n");
        exit(EXIT_FAILURE);
    }

    newNode->t_Node = type;
    
    newNode->sym = symbol; // NULL is allowed for non-terminal nodes

    newNode->lineNo = 0;

    newNode->left = left;
    newNode->right = right;
    newNode->third = third;

    return newNode;
}

// Estructura para almacenar líneas de información de símbolos
#define MAX_SYMBOL_LINES 20
#define MAX_SYMBOL_LINE_LENGTH 256

typedef struct {
    char lines[MAX_SYMBOL_LINES][MAX_SYMBOL_LINE_LENGTH];
    int lineCount;
} SymbolInfoBuffer;

// Función auxiliar para obtener el tipo como string
const char* getSymbolTypeString(infoType type) {
    switch (type) {
        case TYPE_VOID: return "void";
        case TYPE_INTEGER: return "int";
        case TYPE_BOOL: return "bool";
        case NON_TYPE: return "non-type";
        default: return "unknown";
    }
}

// Función auxiliar para obtener el flag como string
const char* getSymbolFlagString(flagType flag) {
    switch (flag) {
        case METH: return "METHOD";
        case VAR: return "VAR";
        case PARAMET: return "PARAM";
        case CONST: return "CONST";
        default: return "UNKNOWN";
    }
}

// Función para generar las líneas de información del símbolo
void generateSymbolInfoBuffer(Symbol* sym, SymbolInfoBuffer* buffer) {
    buffer->lineCount = 0;
    
    if (!sym) return;
    
    // Color según el tipo de símbolo
    const char* color;
    switch (sym->flag) {
        case METH: color = "\033[1;34m"; break;   // Azul para métodos
        case VAR: color = "\033[1;32m"; break;     // Verde para variables
        case PARAMET: color = "\033[1;35m"; break; // Magenta para parámetros
        case CONST: color = "\033[1;33m"; break;   // Amarillo para constantes
        default: color = "\033[1;37m"; break;
    }
    
    // Línea 1: Flag
    snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
             "[Flag: %s%s\033[0m]", color, getSymbolFlagString(sym->flag));
    
    // Línea 2: Nombre (si existe)
    if (sym->name) {
        snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
                 "[Name: %s%s\033[0m]", color, sym->name);
    }
    
    // Línea 3: Tipo
    snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
             "[Type: %s%s\033[0m]", color, getSymbolTypeString(sym->type));
    
    // Línea 4: Valor (si aplica)
    if ((sym->flag == VAR || sym->flag == CONST || sym->flag == PARAMET) && 
        (sym->type == TYPE_INTEGER || sym->type == TYPE_BOOL)) {
        snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
                 "[Value: %s%d\033[0m]", color, sym->value);
    }
    
    // Línea 5: Parámetros (si es un método con parámetros)
    if (sym->flag == METH && sym->nextParam) {
        snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
                 "[Params:");
        
        Symbol* param = sym->nextParam;
        int paramNum = 1;
        while (param) {
            // Mostrar nombre y tipo del parámetro
            if (param->type == TYPE_INTEGER || param->type == TYPE_BOOL) {
                // Si tiene un tipo con valor, mostrar también el valor
                snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
                         "  %d. %s%s\033[0m (%s%s\033[0m) = %s%d\033[0m", 
                         paramNum++,
                         color, param->name ? param->name : "unnamed",
                         color, getSymbolTypeString(param->type),
                         color, param->value);
            } else {
                // Para tipos sin valor (void, etc.)
                snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH,
                         "  %d. %s%s\033[0m (%s%s\033[0m)", 
                         paramNum++,
                         color, param->name ? param->name : "unnamed",
                         color, getSymbolTypeString(param->type));
            }
            param = param->nextParam;
        }
        
        snprintf(buffer->lines[buffer->lineCount++], MAX_SYMBOL_LINE_LENGTH, "]");
    }
}

// Función auxiliar para imprimir el prefijo del árbol
void printTreePrefix(int* isLast, int level) {
    for (int i = 0; i < level - 1; i++) {
        if (isLast[i]) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }
    if (level > 0) {
        if (isLast[level - 1]) {
            printf("└── ");
        } else {
            printf("├── ");
        }
    }
}

// Función auxiliar para imprimir líneas adicionales (información de símbolos)
void printAdditionalLines(int* isLast, int level, const char* line) {
    // Imprimir la estructura del árbol manteniendo la alineación
    for (int i = 0; i < level - 1; i++) {
        if (isLast[i]) {
            printf("    ");
        } else {
            printf("│   ");
        }
    }
    
    // Agregar el prefijo del árbol antes de la información del símbolo
    if (level > 0) {
        if (isLast[level - 1]) {
            printf("    ");  // Espacios si es el último hijo
        } else {
            printf("│   ");  // Barra vertical si no es el último
        }
    }
    
    // Añadir el marcador visual para la información del símbolo
    printf("│   %s\n", line);
}

// Función auxiliar para contar hijos no nulos
int countChildren(Node* node) {
    if (!node) return 0;
    int count = 0;
    if (node->left) count++;
    if (node->right) count++;
    if (node->third) count++;
    return count;
}

void printASTHelper(Node* root, int* isLast, int level) {
    if (!root) return;

    // Generar buffer de información del símbolo si existe
    SymbolInfoBuffer symBuffer;
    symBuffer.lineCount = 0;
    if (root->sym) {
        generateSymbolInfoBuffer(root->sym, &symBuffer);
    }

    // Imprimir el prefijo del árbol
    printTreePrefix(isLast, level);

    // Imprimir el nodo actual con colores y mejor formato
    switch (root->t_Node) {
        case N_PROG:
            printf("\033[1;36mPROGRAM\033[0m\n");
            break;
        case N_VAR_DECL:
            printf("\033[1;35mVAR_DECLARATION\033[0m\n");
            break;
        case N_METHOD_DECL:
            printf("\033[1;34mMETHOD_DECLARATION\033[0m\n");
            break;
        case N_STATEMENT:
            printf("\033[1;33mSTATEMENT\033[0m\n");
            break;
        case N_EXPR:
            printf("\033[1;32mEXPRESSION\033[0m\n");
            break;
        case N_ASSIGN:
            printf("\033[1;34mASSIGNMENT\033[0m\n");
            break;
        case N_RETURN:
            printf("\033[1;35mRETURN\033[0m\n");
            break;
        case N_BLOCK:
            printf("\033[1;33mBLOCK\033[0m\n");
            break;
        case N_METHOD_CALL:
            printf("\033[1;36mMETHOD_CALL\033[0m\n");
            break;
        case N_EXTERN:
            printf("\033[1;35mEXTERN\033[0m\n");
            break;
        case N_WHILE:
            printf("\033[1;33mWHILE\033[0m\n");
            break;
        case N_IF:
            printf("\033[1;33mIF\033[0m\n");
            break;
        case N_THEN:
            printf("\033[1;33mTHEN\033[0m\n");
            break;
        case N_ELSE:
            printf("\033[1;33mELSE\033[0m\n");
            break;
        case N_TERM:
            printf("\033[1;94mTERM\033[0m\n");
            break;
        case N_PLUS:
            printf("\033[1;33mOPERATOR\033[0m +\n");
            break;
        case N_MINUS:
            printf("\033[1;33mOPERATOR\033[0m -\n");
            break;
        case N_MULT:
            printf("\033[1;33mOPERATOR\033[0m *\n");
            break;
        case N_DIV:
            printf("\033[1;33mOPERATOR\033[0m /\n");
            break;
        case N_MOD:
            printf("\033[1;33mOPERATOR\033[0m %%\n");
            break;
        case N_LESS:
            printf("\033[1;33mOPERATOR\033[0m <\n");
            break;
        case N_GREAT:
            printf("\033[1;33mOPERATOR\033[0m >\n");
            break;
        case N_EQUAL:
            printf("\033[1;33mOPERATOR\033[0m ==\n");
            break;
        case N_OR:
            printf("\033[1;33mOPERATOR\033[0m ||\n");
            break;
        case N_NOT:
            printf("\033[1;33mOPERATOR\033[0m !\n");
            break;
        case N_NEG:
            printf("\033[1;33mOPERATOR\033[0m - (unary)\n");
            break;
        default:
            printf("\033[1;31mUNKNOWN\033[0m [type: %d]\n", root->t_Node);
    }

    // Imprimir información del símbolo si existe
    if (symBuffer.lineCount > 0) {
        for (int i = 0; i < symBuffer.lineCount; i++) {
            printAdditionalLines(isLast, level, symBuffer.lines[i]);
        }
    }

    // Procesar hijos en orden
    Node* children[3] = {root->left, root->right, root->third};
    int childCount = countChildren(root);
    int currentChild = 0;

    for (int i = 0; i < 3; i++) {
        if (children[i]) {
            currentChild++;
            isLast[level] = (currentChild == childCount);
            printASTHelper(children[i], isLast, level + 1);
        }
    }
}

void printAST(Node* root, int level) {
    if (!root) {
        printf("\033[1;31m(Empty AST)\033[0m\n");
        return;
    }
    
    printf("\n\033[1;36m=== ABSTRACT SYNTAX TREE ===\033[0m\n\n");
    
    int isLast[100] = {0}; // Asumiendo profundidad máxima de 100
    printASTHelper(root, isLast, level);
    
    printf("\n\033[1;36m============================\033[0m\n\n");
}

// Función auxiliar para liberar símbolos una sola vez
static void freeASTHelper(Node* root, Symbol*** freedSymbols, int* freedCount, int* freedCapacity) {
    if (!root) return;

    // Liberar los nodos hijos primero
    freeASTHelper(root->left, freedSymbols, freedCount, freedCapacity);
    freeASTHelper(root->right, freedSymbols, freedCount, freedCapacity);
    freeASTHelper(root->third, freedSymbols, freedCount, freedCapacity);

    // Liberar el símbolo solo si no ha sido liberado antes
    if (root->sym) {
        // Verificar si ya fue liberado
        int alreadyFreed = 0;
        for (int i = 0; i < *freedCount; i++) {
            if ((*freedSymbols)[i] == root->sym) {
                alreadyFreed = 1;
                break;
            }
        }
        
        if (!alreadyFreed) {
            // Expandir el array si es necesario
            if (*freedCount >= *freedCapacity) {
                *freedCapacity *= 2;
                *freedSymbols = (Symbol**) realloc(*freedSymbols, (*freedCapacity) * sizeof(Symbol*));
            }
            
            // Agregar al array de símbolos liberados
            (*freedSymbols)[*freedCount] = root->sym;
            (*freedCount)++;
            
            // Liberar el símbolo
            freeSymbol(root->sym);
        }
    }

    // Liberar el nodo
    free(root);
}

void freeAST(Node* root) {
    if (!root) return;
    
    // Array dinámico para rastrear símbolos liberados
    int freedCapacity = 100;
    int freedCount = 0;
    Symbol** freedSymbols = (Symbol**) malloc(freedCapacity * sizeof(Symbol*));
    
    freeASTHelper(root, &freedSymbols, &freedCount, &freedCapacity);
    
    free(freedSymbols);
}