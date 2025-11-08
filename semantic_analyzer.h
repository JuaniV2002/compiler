// semantic_analyzer.h
#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "symbol.h"
#include "ast.h"
#include "ts.h"

infoType findType(Node* root);

Symbol* insertLastInList(Symbol* list, Symbol* newSym);

Symbol* inOrderExpressionList(Node* argNode, Symbol* exprList);

int checkParameters(Node* methodCall, Stack* stack);

void fullCheck(Node* root, Stack* stack);

int analyzeSemantics(Node* root, Stack* stack);

#endif // SEMANTIC_ANALYZER_H