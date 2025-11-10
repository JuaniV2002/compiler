---
title: "Verificaciones Semánticas"
description: "Reglas de validación implementadas."
---

# Verificaciones Semánticas

## findType()

Determina el tipo de una expresión:

```c
infoType findType(Node* root) {
    if (!root) return NON_TYPE;

    switch (root->t_Node) {
        case N_TERM:        // Literal o variable, retornar el tipo del símbolo
            return root->sym->type;

        case N_PLUS:    // Operadores aritméticos, lógicos e invocaciones a métodos
        ...
        case N_AND:     // Retornar el tipo del símbolo o buscar en hijos
            if (root->sym) return root->sym->type;

            if (root->left) {
                infoType type = findType(root->left);
                if (type != NON_TYPE) return type;
            }

            if (root->right) {
                infoType type = findType(root->right);
                if (type != NON_TYPE) return type;
            }

            return NON_TYPE;    // Si no se encontró un tipo, retorna el valor por defecto (funciona como null)
        default:
            fprintf(stderr, "Error funcion 'findType()': Nodo del AST no soportado.\n");
            exit(EXIT_FAILURE);
    }
}
```

## checkParameters()

Valida llamadas a métodos:

```c
int checkParameters(Node* methodCall, Level* symbolTable) {
    Symbol* method = getSymbol(symbolTable, methodCall->sym->name);
    Symbol* param = method->nextParam;
    Symbol* exprList = inOrderExpressionList(methodCall->left, NULL);
    
    // Comparar tipos y cantidad
    while (param && currentExpr) {
        if (param->type != currentExpr->type) return 0;

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
```

## fullCheck()

Función principal que valida todo el AST:

```c
void fullCheck(Node* root, Stack* stack) {
    if (!root || !stack) return;

    switch (root->t_Node) {
        case N_PROG:
        case N_STATEMENT:
            // 1. Procesar hijo/s
        case N_VAR_DECL:
            if (root->sym) {
                // 1. Verificar que el tipo de la variable y el de la expresion a asignar sean iguales
                // 2. Insertar variable con un nuevo símbolo en la tabla de símbolos
                // 3. Liberar memoria del símbolo anterior del ast despues de insertarlo en la tabla de símbolos
                // 4. Vincular el símbolo del nodo con el de la tabla de símbolos
            } else {
                // 1. Si no hay simbolo, es un nodo intermedio que contiene otras declaraciones
                // 2. Procesar hijo/s
            }
        case N_METHOD_DECL:
            if (root->sym) {
                // 1. Insertar método en la tabla de símbolos
                // 2. Copiar los parametros del símbolo del AST al símbolo de la tabla

                // 3. Comprobar que el método 'main' no tenga errores 
                //      Error semantico: El método 'main' no debe tener parametros, no puede ser externo.
                //      debe tener tipo de retorno 'void', solo puede haber un método 'main'.

                // 4. Si el método no es externo, analizar su cuerpo
                //      Los parámetros ya están en newSym, ahora verificarlos
                //      Analizar las declaraciones del cuerpo del metodo

                // 5. Liberar memoria del símbolo anterior del ast déspues de insertarlo en la tabla de símbolos
                // 6. Vincular el símbolo del nodo con el de la tabla de símbolos
            } else {
                // 1. Si no hay simbolo, es un nodo intermedio que contiene otros métodos
                // 2. Procesar hijo/s
            }
        case N_BLOCK:
            // 1. Se abre un nuevo nivel de la tabla de símbolos
            // 2. Procesar hijo/s
            // 3. Se cierra el nuevo nivel de la tabla de símbolos luego de analizar su cuerpo
        case N_ASSIGN:
            // 1. Procesar los hijos para vincular los símbolos
            // 2. Verificar errores
            //      Error semantico: Variable 'nombre' no declarada. Asignacion con tipos incompatibles.
            //      Error semantico: El símbolo 'nombre' no es una variable o parametro y no se le puede asignar un valor.
            // 3. Liberar memoria del símbolo anterior del ast. Vincular el símbolo del nodo con el de la tabla de símbolos
        case N_METHOD_CALL:
            // 1. Procesar los argumentos para vincular los símbolos
            // 2. Verificar errores
            //      Error semantico: Metodo 'nombre' no declarado. El símbolo 'nombre' no es un método y no se le puede invocar.
            //      Error semantico: Llamada a método inexistente 'nombre' o con parametros incorrectos.
            // 3. Liberar memoria del símbolo anterior del ast. Vincular el símbolo del nodo con el de la tabla de símbolos
        case N_EXPR:
            // 1. Es una expresion terminal (constante o variable)
            // 2. Variable, buscar en la tabla de símbolos
            //       Error semantico: Variable 'nombre' no declarada.
            //       Error semantico: El símbolo 'nombre' no es una variable o parametro y no se puede usar como expresion.
            //       Liberar memoria del símbolo anterior del ast. Vincular el símbolo del nodo con el de la tabla de símbolos

            // 3. Error semantico: El símbolo 'nombre' no es una constante, variable o parametro valido para una expresion.

            // 4. Procesar hijo/s
        case N_PLUS: case N_MINUS: case N_MULT: case N_DIV: case N_MOD:
        case N_LESS: case N_GREAT: case N_EQUAL: case N_AND: case N_OR: case N_NOT: case N_NEG:
            // 1. Verificar errores
            //      Error semantico: Operacion 'nombre operacion' incompleta.
            //      Error semantico: Variable 'nombre' no declarada.
            //      Error semantico: Operacion 'nombre operacion' con tipos no enteros o booleanos.
            // 2. Establecer el tipo de resultado como entero o booleano
        case N_IF:
        case N_WHILE:
            // 1. Procesar la condición para vincular los símbolos
            // 2. Verificar errores
            //      Error semantico: La expresion condicional 'nombre operacion' no es booleana.
            // 3. Procesar hijo/s
        case N_THEN:
        case N_ELSE:
            // Procesar hijo
        case N_RETURN:
            // 1. Procesar la expresión de retorno si existe
            // 2. Verificar errores
            //      Error de retorno: Tipos incompatibles en el retorno del método y la expresion de retorno.
            //      Error de retorno: El método debe retornar un valor.
        case N_TERM:
            // 1. Si no es una constante, buscar en la tabla de símbolos
            // 2. Verificar errores
            //      Error semantico: Variable 'nombre' no declarada.
        default:
            // Error semantico: Nodo desconocido en el AST
    }
}
```
