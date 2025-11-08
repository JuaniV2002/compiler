// symbol.h
#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum {METH, VAR, PARAMET, CONST} flagType;

typedef enum { TYPE_VOID, TYPE_INTEGER, TYPE_BOOL, NON_TYPE } infoType;

typedef struct Symbol {
    flagType flag;
    infoType type;
    char* name;     // Solo para un simbolo con flag distinto de CONST
    int value;      // Valor de variables, parametros y constantes o valor de retorno para metodos

    struct Symbol* nextParam;  // Para la lista enlazada de parametros de los metodos
} Symbol;

// Crea un nuevo simbolo con los atributos especificados
Symbol* newSymbol(flagType flag, infoType type, char* name, int value);

// Agrega un parametro a la lista de parametros de un metodo
Symbol* addParameter(Symbol* method, Symbol* param);

// Crea y agrega un nuevo parametro a un metodo
Symbol* newParameter(Symbol* method, infoType type, char* name, int value);

// Crea y agrega un parametro durante una llamada a metodo
Symbol* newParameterCall(Symbol* method, Symbol* param);

// Busca un parametro por nombre en la lista de parametros de un metodo
Symbol* getParameter(Symbol* method, char* name);

// Imprime la informacion de un simbolo
void printSymbol(Symbol* sym);

// Libera la memoria de un simbolo
void freeSymbol(Symbol* sym);

// Libera una lista completa de simbolos
void freeSymbolList(Symbol* list);

#endif // SYMBOL_H