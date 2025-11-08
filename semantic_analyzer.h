// semantic_analyzer.h
#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "symbol.h"
#include "ast.h"
#include "ts.h"

// Determina el tipo de una expresion
infoType findType(Node* root);

// Inserta un simbolo al final de una lista
Symbol* insertLastInList(Symbol* list, Symbol* newSym);

// Construye una lista de expresiones en orden desde los argumentos
Symbol* inOrderExpressionList(Node* argNode, Symbol* exprList);

// Verifica que los parametros de una llamada coincidan con la declaracion
int checkParameters(Node* methodCall, Stack* stack);

// Realiza todas las verificaciones semanticas sobre el AST
void fullCheck(Node* root, Stack* stack);

// Ejecuta el analisis semantico completo y retorna 1 si es exitoso
int analyzeSemantics(Node* root, Stack* stack);

#endif // SEMANTIC_ANALYZER_H