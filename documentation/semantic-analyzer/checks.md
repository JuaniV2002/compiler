---
title: "Verificaciones Semánticas"
description: "Reglas de validación implementadas."
---

# Verificaciones Semánticas

## findType()

Determina el tipo de una expresión:

```c
infoType findType(Node* root) {
    switch (root->t_Node) {
        case N_TERM:
            return root->sym->type;  // Literal o variable
        case N_PLUS:
        case N_MULT:
            // Operadores aritméticos → buscar en hijos
            if (root->sym) return root->sym->type;
            if (root->left) {
                infoType type = findType(root->left);
                if (type != NON_TYPE) return type;
            }

            if (root->right) {
                infoType type = findType(root->right);
                if (type != NON_TYPE) return type;
            }
        // ... más casos
    }
}
```

## checkReturn()

Verifica que los métodos tengan return:

```c
int checkReturn(Node* root, Level* symbolTable) {
    switch (root->t_Node) {
        case N_RETURN:
            // Validar tipo
            return 1;
        case N_IF:
            // Ambos bloques deben tener return
            thenHasReturn = checkReturn(root->right, symbolTable);
            elseHasReturn = checkReturn(root->third, symbolTable);
            return thenHasReturn && elseHasReturn;
    }
}
```

:::callout warning
If sin else no cuenta como return completo.
:::

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
    
    return !(param || currentExpr);  // True si coinciden
}
```

## fullCheck()

Función principal que valida todo el AST:

```c
void fullCheck(Node* root, Level* symbolTable) {
    switch (root->t_Node) {
        case N_VAR_DECL:
            insertSymbol(...);
            break;
        case N_METHOD_DECL:
            openNewLevel();
            // Agregar parámetros
            // Verificar cuerpo
            // Validar return
            closeLevel();
            break;
    }
}
```
