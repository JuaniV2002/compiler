// semantic_analyzer.h
#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "symbol.h"
#include "ast.h"
#include "ts.h"

infoType findType(Node* root);

int checkReturn(Node* root, Level* symbolTable);

Symbol* insertLastInList(Symbol* list, Symbol* newSym);

Symbol* inOrderExpressionList(Node* argNode, Symbol* exprList);

int checkParameters(Node* methodCall, Level* symbolTable);

void fullCheck(Node* root, Level* symbolTable);

int analyzeSemantics(Node* root, Level* symbolTable);

#endif // SEMANTIC_ANALYZER_H