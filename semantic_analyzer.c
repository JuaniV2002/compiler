#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"
#include "ts.h"
#include "semantic_analyzer.h"

infoType retType = NON_TYPE;
int mainMethodDeclared = 0;

// TODO: Devolver nro de linea en error semantico

infoType findType(Node* root) {
    if (!root) return NON_TYPE;

    switch (root->t_Node) {
        case N_METHOD_CALL:
        case N_EXPR:
        case N_PLUS:
        case N_MINUS:
        case N_MULT:
        case N_DIV:
        case N_MOD:
        case N_LESS:
        case N_GREAT:
        case N_EQUAL:
        case N_AND:
        case N_OR:
        case N_NOT:
        case N_NEG:
            if (root->sym) {
                return root->sym->type;
            }

            if (root->left) {
                infoType type = findType(root->left);
                if (type != NON_TYPE) return type;
            }

            if (root->right) {
                infoType type = findType(root->right);
                if (type != NON_TYPE) return type;
            }

            return NON_TYPE;
        case N_TERM:
            return root->sym->type;
        default:
            fprintf(stderr, "Error funcion 'findType()': Nodo del AST no soportado '%s'.\n", root->t_Node == N_PROG ? "N_PROG" :
                                                                                             root->t_Node == N_VAR_DECL ? "N_VAR_DECL" :
                                                                                             root->t_Node == N_METHOD_DECL ? "N_METHOD_DECL" :
                                                                                             root->t_Node == N_BLOCK ? "N_BLOCK" :
                                                                                             root->t_Node == N_EXTERN ? "N_EXTERN" :
                                                                                             root->t_Node == N_STATEMENT ? "N_STATEMENT" :
                                                                                             root->t_Node == N_ASSIGN ? "N_ASSIGN" :
                                                                                             root->t_Node == N_IF ? "N_IF" :
                                                                                             root->t_Node == N_THEN ? "N_THEN" :
                                                                                             root->t_Node == N_ELSE ? "N_ELSE" :
                                                                                             root->t_Node == N_WHILE ? "N_WHILE" :
                                                                                             root->t_Node == N_RETURN ? "N_RETURN" : "DESCONOCIDO");
            exit(EXIT_FAILURE);
            break;
    }

    return NON_TYPE; // Tipo desconocido o no aplicable
}

Symbol* insertLastInList(Symbol* list, Symbol* newSym) {
    if (!list) return newSym;

    Symbol* current = list;
    while (current->nextParam) {
        current = current->nextParam;
    }
    current->nextParam = newSym;

    return list;
}

Symbol* inOrderExpressionList(Node* argNode, Symbol* exprList) {
    if (!argNode) return exprList;

    // Booleano que indica si el nodo es un nodo no terminal del arbol de expresiones
    int nonTerminal = ((argNode->t_Node == N_EXPR || argNode->t_Node == N_PLUS ||
                        argNode->t_Node == N_MINUS || argNode->t_Node == N_MULT ||
                        argNode->t_Node == N_DIV || argNode->t_Node == N_MOD ||
                        argNode->t_Node == N_LESS || argNode->t_Node == N_GREAT ||
                        argNode->t_Node == N_EQUAL || argNode->t_Node == N_AND ||
                        argNode->t_Node == N_OR || argNode->t_Node == N_NOT || argNode->t_Node == N_NEG) && !argNode->sym);

    // Si es un nodo no terminal del arbol de expresiones (no tiene símbolo), recorrer
    if (nonTerminal) {
        exprList = inOrderExpressionList(argNode->left, exprList);
        exprList = inOrderExpressionList(argNode->right, exprList);
        return exprList;
    }

    // Si llegamos aquí, es una expresión terminal (argumento de la función)
    // Obtener el tipo de la expresión completa
    infoType exprType = findType(argNode);
    Symbol* newSym = newSymbol(CONST, exprType, NULL, 0);   // Solo importa el tipo para la verificacion de parametros

    exprList = insertLastInList(exprList, newSym);

    return exprList;
}

int checkParameters(Node* methodCall, Stack* stack) {
    if (!methodCall || !methodCall->sym || !stack) return 0;

    Symbol* method = getSymbol(stack, methodCall->sym->name);
    if (!method) return 0;

    Symbol* param = method->nextParam;

    Symbol* exprList = NULL;
    exprList = inOrderExpressionList(methodCall->left, exprList);

    Symbol* currentExpr = exprList;
    while (param && currentExpr) {
        if (param->type != currentExpr->type) {
            freeSymbolList(exprList);   // Liberar toda la lista de expresiones
            return 0;
        }

        param = param->nextParam;
        currentExpr = currentExpr->nextParam;
    }

    // Si quedaron parametros sin argumentos o argumentos sin parametros, no coinciden
    if (param || currentExpr) {
        freeSymbolList(exprList);   // Liberar toda la lista de expresiones
        return 0;
    }

    freeSymbolList(exprList);   // Liberar toda la lista de expresiones
    return 1;   // Los parametros coinciden
}

Symbol* replaceSymbolNonConst(Node* node, Stack* stack) {
    if (!node || !node->sym || !stack) return NULL;

    Symbol* symInTable = getSymbol(stack, node->sym->name);
    if (!symInTable) {
        fprintf(stderr, "Error semantico: El simbolo '%s' no existe en la tabla de simbolos.\n", node->sym->name);
        return NULL;
    }

    return symInTable;
}

void fullCheck(Node* root, Stack* stack) {
    if (!root || !stack) return;

    infoType leftType;
    infoType rightType;

    switch (root->t_Node) {
        case N_PROG:
            if (root->left) fullCheck(root->left, stack);
            if (root->right) fullCheck(root->right, stack);
            break;
        case N_VAR_DECL:
            if (root->sym) {
                if (root->sym->type != findType(root->left)) {
                    fprintf(stderr, "Error semantico: Tipo de variable '%s' no coincide con el tipo de la expresion.\n", root->sym->name);
                    exit(EXIT_FAILURE);
                }

                // Insertar variable en la tabla de simbolos
                Symbol* newSym = insertSymbol(stack, root->sym->flag, root->sym->type, root->sym->name, root->sym->value);
                if (!newSym) {
                    fprintf(stderr, "Error semantico: No se pudo insertar la variable '%s'.\n", root->sym->name);
                    exit(EXIT_FAILURE);
                }
                freeSymbol(root->sym);  // Liberar memoria del simbolo anterior del ast despues de insertarlo en la tabla de simbolos
                root->sym = newSym;     // Vincular el simbolo del nodo con el de la tabla de simbolos
            } else {
                fullCheck(root->left, stack);
                fullCheck(root->right, stack);
            }
            break;
        case N_METHOD_DECL:
            if (root->sym) {
                // Insertar metodo en la tabla de simbolos
                Symbol* newSym = insertSymbol(stack, root->sym->flag, root->sym->type, root->sym->name, root->sym->value);
                if (!newSym) {
                    fprintf(stderr, "Error semantico: No se pudo insertar el metodo '%s'.\n", root->sym->name);
                    exit(EXIT_FAILURE);
                }

                // Copiar los parametros del símbolo del AST al símbolo de la tabla
                Symbol* param = root->sym->nextParam;
                while (param) {
                    Symbol* newParam = newParameter(newSym, param->type, param->name, param->value);
                    if (!newParam) {
                        fprintf(stderr, "Error semantico: No se pudo agregar el parametro '%s' al metodo '%s'.\n", param->name, newSym->name);
                        exit(EXIT_FAILURE);
                    }
                    param = param->nextParam;
                }

                // Liberar los parámetros originales del AST ya que fueron copiados a la tabla
                freeSymbolList(root->sym->nextParam);
                root->sym->nextParam = NULL;

                int mainName = (strcmp(newSym->name, "main") == 0);
                
                if (mainName) {
                    if (newSym->nextParam != NULL) {
                        fprintf(stderr, "Error semantico: El metodo 'main' no debe tener parametros.\n");
                        exit(EXIT_FAILURE);
                    }

                    if (root->left->t_Node == N_EXTERN) {
                        fprintf(stderr, "Error semantico: El metodo 'main' no puede ser externo.\n");
                        exit(EXIT_FAILURE);
                    }

                    if (newSym->type != TYPE_VOID) {
                        fprintf(stderr, "Error semantico: El metodo 'main' debe tener tipo de retorno 'void'.\n");
                        exit(EXIT_FAILURE);
                    }

                    mainMethodDeclared++;

                    if (mainMethodDeclared > 1) {
                        fprintf(stderr, "Error semantico: Solo puede haber un metodo 'main'.\n");
                        exit(EXIT_FAILURE);
                    }
                }

                // Si el metodo no es externo, analizar su cuerpo
                if (root->left->t_Node != N_EXTERN) {
                    // Los parámetros ya están en newSym, ahora verificarlos
                    Symbol* param = newSym->nextParam;
                    while (param) {
                        if (param->type == TYPE_VOID) {
                            fprintf(stderr, "Error semantico: El parametro '%s' del metodo '%s' no puede ser de tipo 'void'.\n", param->name, newSym->name);
                            exit(EXIT_FAILURE);
                        }
                        param = param->nextParam;
                    }

                    retType = newSym->type;

                    // Analizar las declaraciones del cuerpo del metodo
                    if (root->left) fullCheck(root->left, stack);
                }

                freeSymbol(root->sym);  // Liberar memoria del simbolo anterior del ast despues de insertarlo en la tabla de simbolos
                root->sym = newSym;     // Vincular el simbolo del nodo con el de la tabla de simbolos

                retType = NON_TYPE;     // Resetear tipo de retorno despues de analizar el metodo
            } else {
                // Si no hay simbolo, es un nodo intermedio que contiene otros metodos
                if (root->left) fullCheck(root->left, stack);
                if (root->right) fullCheck(root->right, stack);
            }

            break;
        case N_BLOCK:
            openNewLevel(stack);

            if (root->left) fullCheck(root->left, stack);
            if (root->right) fullCheck(root->right, stack);

            closeLevel(stack);
            break;
        case N_EXTERN:
            return;
        case N_STATEMENT:
            if (root->left) fullCheck(root->left, stack);
            if (root->right) fullCheck(root->right, stack);
            break;
        case N_ASSIGN:
            // Primero procesar los hijos para vincular los símbolos
            if (root->left) fullCheck(root->left, stack);
            if (root->right) fullCheck(root->right, stack);

            Symbol* var = getSymbol(stack, root->left->sym->name);
            if (!var) {
                fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                exit(EXIT_FAILURE);
            }
            if (var->flag != VAR && var->flag != PARAMET) {
                fprintf(stderr, "Error semantico: El simbolo '%s' no es una variable o parametro y no se le puede asignar un valor.\n", var->name);
                exit(EXIT_FAILURE);
            }

            rightType = findType(root->right);
            if (var->type != rightType) {
                fprintf(stderr, "Error semantico: Asignacion con tipos incompatibles.\n");
                exit(EXIT_FAILURE);
            }
            
            freeSymbol(root->sym);  // Liberar memoria del simbolo anterior del ast
            root->sym = var;        // Vincular el simbolo del nodo con el de la tabla de simbolos
            break;
        case N_METHOD_CALL:
            // Primero procesar los argumentos para vincular los símbolos
            if (root->left) fullCheck(root->left, stack);

            Symbol* method = getSymbol(stack, root->sym->name);
            if (!method) {
                fprintf(stderr, "Error semantico: Metodo '%s' no declarado.\n", root->sym->name ? root->sym->name : "unknown");
                exit(EXIT_FAILURE);
            }
            if (method->flag != METH) {
                fprintf(stderr, "Error semantico: El simbolo '%s' no es un metodo y no se le puede invocar.\n", method->name);
                exit(EXIT_FAILURE);
            }

            if (!checkParameters(root, stack)) {
                fprintf(stderr, "Error semantico: Llamada a metodo inexistente '%s' o con parametros incorrectos.\n", method->name);
                exit(EXIT_FAILURE);
            }

            freeSymbol(root->sym);  // Liberar memoria del simbolo anterior del ast
            root->sym = method;     // Vincular el simbolo del nodo con el de la tabla de simbolos
            break;
        case N_EXPR:
            // Es una expresion terminal (constante o variable)
            if (root->sym) {
                if (root->sym->flag == VAR || root->sym->flag == PARAMET) {
                    // Variable, buscar en la tabla de simbolos
                    Symbol* varSym = getSymbol(stack, root->sym->name);
                    if (!varSym) {
                        fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->sym->name);
                        exit(EXIT_FAILURE);
                    }
                    if (varSym->flag != VAR && varSym->flag != PARAMET) {
                        fprintf(stderr, "Error semantico: El simbolo '%s' no es una variable o parametro y no se puede usar como expresion.\n", varSym->name);
                        exit(EXIT_FAILURE);
                    }

                    freeSymbol(root->sym);  // Liberar memoria del simbolo anterior del ast
                    root->sym = varSym;     // Vincular el simbolo del nodo con el de la tabla de simbolos
                } else {
                    fprintf(stderr, "Error semantico: El simbolo '%s' no es una constante, variable o parametro valido para una expresion.\n", root->sym->name ? root->sym->name : "unknown");
                    exit(EXIT_FAILURE);
                }
            }

            if (root->left) fullCheck(root->left, stack);
            if (root->right) fullCheck(root->right, stack);
            break;
        case N_PLUS:
        case N_MINUS:
        case N_MULT:
        case N_DIV:
        case N_MOD:
            if (!root->left || !root->right) {
                fprintf(stderr, "Error semantico: Operacion aritmetica '%s' incompleta.\n", root->t_Node == N_PLUS ? "suma" :
                                                                                                  root->t_Node == N_MINUS ? "resta" :
                                                                                                  root->t_Node == N_MULT ? "multiplicacion" :
                                                                                                  root->t_Node == N_DIV ? "division" :
                                                                                                  root->t_Node == N_MOD ? "modulo" : "DESCONOCIDO");
                exit(EXIT_FAILURE);
            }
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (root->right->sym->flag != CONST) {
                root->right->sym = replaceSymbolNonConst(root->right, stack);
                if (!root->right->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->right->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            leftType = findType(root->left);
            rightType = findType(root->right);
            if (leftType != TYPE_INTEGER || rightType != TYPE_INTEGER) {
                fprintf(stderr, "Error semantico: Operacion aritmetica '%s' con tipos no enteros.\n", root->t_Node == N_PLUS ? "suma" :
                                                                                                      root->t_Node == N_MINUS ? "resta" :
                                                                                                      root->t_Node == N_MULT ? "multiplicacion" :
                                                                                                      root->t_Node == N_DIV ? "division" :
                                                                                                      root->t_Node == N_MOD ? "modulo" : "DESCONOCIDO");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como entero
            root->sym = newSymbol(CONST, TYPE_INTEGER, NULL, 0);
            break;
        case N_LESS:
        case N_GREAT:
            if (!root->left || !root->right) {
                fprintf(stderr, "Error semantico: Operacion relacional '%s' incompleta.\n", root->t_Node == N_LESS ? "menor" :
                                                                                                   root->t_Node == N_GREAT ? "mayor" : "DESCONOCIDO");
                exit(EXIT_FAILURE);
            }
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (root->right->sym->flag != CONST) {
                root->right->sym = replaceSymbolNonConst(root->right, stack);
                if (!root->right->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->right->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            leftType = findType(root->left);
            rightType = findType(root->right);
            if (leftType != TYPE_INTEGER || rightType != TYPE_INTEGER) {
                fprintf(stderr, "Error semantico: Operacion relacional '%s' con tipos no enteros.\n", root->t_Node == N_LESS ? "menor" :
                                                                                                      root->t_Node == N_GREAT ? "mayor" : "DESCONOCIDO");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como booleano
            root->sym = newSymbol(CONST, TYPE_BOOL, NULL, 0);
            break;
        case N_EQUAL:
            if (!root->left || !root->right) {
                fprintf(stderr, "Error semantico: Operacion relacional 'igual' incompleta.\n");
                exit(EXIT_FAILURE);
            }
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (root->right->sym->flag != CONST) {
                root->right->sym = replaceSymbolNonConst(root->right, stack);
                if (!root->right->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->right->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            leftType = findType(root->left);
            rightType = findType(root->right);
            if (leftType != rightType) {
                fprintf(stderr, "Error semantico: Operacion relacional 'igual' con tipos diferentes.\n");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como booleano
            root->sym = newSymbol(CONST, TYPE_BOOL, NULL, 0);
            break;
        case N_AND:
        case N_OR:
            if (!root->left || !root->right) {
                fprintf(stderr, "Error semantico: Operacion logica '%s' incompleta.\n", root->t_Node == N_AND ? "and" : "or");
                exit(EXIT_FAILURE);
            }
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (root->right->sym->flag != CONST) {
                root->right->sym = replaceSymbolNonConst(root->right, stack);
                if (!root->right->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->right->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            leftType = findType(root->left);
            rightType = findType(root->right);
            if (leftType != TYPE_BOOL || rightType != TYPE_BOOL) {
                fprintf(stderr, "Error semantico: Operacion logica '%s' con tipos no booleanos.\n", root->t_Node == N_AND ? "and" : "or");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como booleano
            root->sym = newSymbol(CONST, TYPE_BOOL, NULL, 0);
            break;
        case N_NOT:
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (findType(root->left) != TYPE_BOOL) {
                fprintf(stderr, "Error semantico: Operacion logica 'not' con tipo no booleano.\n");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como booleano
            root->sym = newSymbol(CONST, TYPE_BOOL, NULL, 0);
            break;
        case N_NEG:
            if (root->left->sym->flag != CONST) {
                root->left->sym = replaceSymbolNonConst(root->left, stack);
                if (!root->left->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->left->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            if (findType(root->left) != TYPE_INTEGER) {
                fprintf(stderr, "Error semantico: Operacion aritmetica unaria 'negativo' con tipo no entero.\n");
                exit(EXIT_FAILURE);
            }
            // Establecer el tipo de resultado como entero
            root->sym = newSymbol(CONST, TYPE_INTEGER, NULL, 0);
            break;
        case N_IF:
            // Primero procesar la condición para vincular los símbolos
            if (root->left) fullCheck(root->left, stack);

            if (findType(root->left) != TYPE_BOOL) {
                fprintf(stderr, "Error semantico: La expresion condicional 'if' no es booleana.\n");
                exit(EXIT_FAILURE);
            }

            if (root->right) fullCheck(root->right, stack);
            if (root->third) fullCheck(root->third, stack);
            break;
        case N_THEN:
            if (root->left) {
                fullCheck(root->left, stack);
            }
            break;
        case N_ELSE:
            if (root->left) fullCheck(root->left, stack);
            break;
        case N_WHILE:
            // Primero procesar la condición para vincular los símbolos
            if (root->left) fullCheck(root->left, stack);

            if (findType(root->left) != TYPE_BOOL) {
                fprintf(stderr, "Error semantico: La expresion condicional 'while' no es booleana.\n");
                exit(EXIT_FAILURE);
            }

            if (root->right) fullCheck(root->right, stack);
            break;
        case N_RETURN:
            // Primero procesar la expresión de retorno si existe
            if (root->left) fullCheck(root->left, stack);

            if (root->left) {
                infoType retExprType = findType(root->left);
                if (retType != retExprType) {
                    fprintf(stderr, "Error de retorno: Tipos incompatibles en el retorno del metodo y la expresion de retorno.\n");
                    exit(EXIT_FAILURE);
                }
            } else if (retType != TYPE_VOID) {
                fprintf(stderr, "Error de retorno: El metodo debe retornar un valor.\n");
                exit(EXIT_FAILURE);
            }
            break;
        case N_TERM:
            // Si no es una constante, buscar en la tabla de simbolos
            if (root->sym->flag != CONST) {
                root->sym = replaceSymbolNonConst(root, stack);
                if (!root->sym) {
                    fprintf(stderr, "Error semantico: Variable '%s' no declarada.\n", root->sym->name);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        default:
            fprintf(stderr, "Error semantico: Nodo desconocido en el AST.\n");
            exit(EXIT_FAILURE);
            break;
    }
}

int analyzeSemantics(Node* root, Stack* stack) {
    if (!root) {
        fprintf(stderr, "Error semantico: El AST es NULL.\n");
        exit(EXIT_FAILURE);
    }

    fullCheck(root, stack);
    if (!mainMethodDeclared) {
        fprintf(stderr, "Error semantico: No se encontro un metodo 'main'.\n");
        exit(EXIT_FAILURE);
    }

    return 0; // No hay errores semanticos
}