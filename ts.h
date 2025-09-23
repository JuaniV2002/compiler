// ts.h
#ifndef TS_H
#define TS_H

#include "ast.h"

typedef enum {METH, VAR, PARAMET} flagType;

typedef struct Symbol {
    char* name;
    infoType type;
    Value* value;
} Symbol;

typedef struct ParamNode {
    flagType flag;
    Symbol* param;

    struct ParamNode* next;
} ParamNode;

typedef struct Method {
    char* name;
    infoType returnType;
    ParamNode* paramList;
} Method;

typedef union {
    Symbol* symbol;
    Method* method;
} SymbolOrMethod;

typedef struct TSNode {
    flagType flag;
    SymbolOrMethod* sm;

    struct TSNode* next;
} TSNode;

typedef struct Level {
    struct TSNode* tsNode;

    int level;
    struct Level* nextLevel;
} Level;

Level* initializeTS();

Symbol* insertSymbol(Level* symbolTable, char* name, infoType type, Value* value);

Method* insertMethod(Level* symbolTable, char* name, infoType returnType);

TSNode* insertTSNode(Level* symbolTable, flagType flag, Symbol* symbol, Method* method);

ParamNode* insertParam(Method* method, char* name, infoType type);

Symbol* getParam(Method* method, char* name);

Symbol* getSymbol(Level* symbolTable, char* name);

Method* getMethod(Level* symbolTable, char* name);

void printTS(Level* symbolTable);

#endif // TS_H