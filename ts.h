// ts.h
#ifndef TS_H
#define TS_H

#include "symbol.h"
#include "ast.h"

typedef struct TSNode {
    Symbol* symbol;

    struct TSNode* next;
} TSNode;

typedef struct Level {
    struct TSNode* tsNode;

    int levelNumber;
    struct Level* nextLevel;
} Level;

Level* initializeTS();

Level* topLevel(Level* firstLevel);

Level* openNewLevel(Level* firstLevel);

void closeLevel(Level* firstLevel);

TSNode* lastTSNode(Level* symbolTable);

Symbol* insertSymbol(Level* symbolTable, flagType flag, infoType type, char* name, int value);

Symbol* insertParameter(Symbol* method, infoType type, char* name, int value);

Symbol* getSymbol(Level* symbolTable, char* name);

void freeTS(Level* symbolTable);

void printTS(Level* symbolTable);

#endif // TS_H