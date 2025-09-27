// symbol.h
#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum {METH, VAR, PARAMET, CONST} flagType;

typedef enum { TYPE_VOID, TYPE_INTEGER, TYPE_BOOL } infoType;

typedef struct Symbol {
    flagType flag;
    infoType type;
    char* name;     // Solo para un simbolo con flag distinto de CONST
    int value;      // Valor de variables, parametros y constantes o valor de retorno para metodos

    struct Symbol* nextParam;  // Para la lista enlazada de parametros de los metodos
} Symbol;

Symbol* newSymbol(flagType flag, infoType type, char* name, int value);

Symbol* newParameter(Symbol* method, infoType type, char* name, int value);

Symbol* getParameter(Symbol* method, char* name);

void printSymbol(Symbol* sym);

void freeSymbol(Symbol* sym);

#endif // SYMBOL_H