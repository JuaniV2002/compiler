---
title: "Generación de TAC"
description: "Algoritmos de traducción desde el AST."
---

# Generación de TAC

## Algoritmo Principal

```c
void generateTAC(Node* root, TacCode* tac) {
    if (root->t_Node == N_PROG) {
        generateMethod(root->right, tac);  // METHOD_DECLS está en right
    }
}
```

Recorre el AST recursivamente y emite instrucciones con `emitTAC()`.

## Generación de Expresiones

`generateExpr()` traduce expresiones y retorna el nombre del temporal o variable:

```c
char* generateExpr(Node* node, TacCode* tac)
```

**Casos**:

- **`N_TERM`**: Retorna constante o nombre de variable
- **`N_PLUS`, `N_MINUS`, etc.**: Genera código para hijos, emite operación, retorna temporal
- **`N_NOT`, `N_NEG`**: Operadores unarios
- **`N_METHOD_CALL`**: Emite `TAC_PARAM` para cada argumento, luego `TAC_CALL`

**Ejemplo** (`x = 3 + 5 * 2`):
```
generateExpr(mul(5, 2))  → t0
generateExpr(add(3, t0)) → t1
```

:::callout tip Post-orden
Los hijos se evalúan antes que el padre, asegurando orden correcto.
:::

## Generación de Sentencias

`generateStmt()` maneja sentencias:

**`N_ASSIGN`**:
```c
char* rval = generateExpr(node->right, tac);
emitTAC(tac, TAC_COPY, node->left->sym->name, rval, NULL);
```

**`N_IF`** (usa `TAC_IF_FALSE` con negación):
```c
emitTAC(tac, TAC_IF_FALSE, NULL, cond, labelElse);
generateStmt(node->right, tac);  // then
if (node->third) {
    emitTAC(tac, TAC_GOTO, labelEnd, NULL, NULL);
    emitTAC(tac, TAC_LABEL, labelElse, NULL, NULL);
    generateStmt(node->third, tac);  // else
}
```

**`N_WHILE`**:
```c
emitTAC(tac, TAC_LABEL, labelStart, NULL, NULL);
char* cond = generateExpr(node->left, tac);
emitTAC(tac, TAC_IF_FALSE, NULL, cond, labelEnd);
generateStmt(node->right, tac);  // body
emitTAC(tac, TAC_GOTO, labelStart, NULL, NULL);
emitTAC(tac, TAC_LABEL, labelEnd, NULL, NULL);
```

**`N_RETURN`**:
```c
char* retVal = generateExpr(node->left, tac);
emitTAC(tac, TAC_RETURN, NULL, retVal, NULL);
```

## Generación de Métodos

```c
void generateMethod(Node* node, TacCode* tac) {
    emitTAC(tac, TAC_BEGIN_FUNC, methodNode->sym->name, NULL, NULL);
    
    // Generar variables y statements del bloque
    generateStmt(methodNode->left, tac);
    
    emitTAC(tac, TAC_END_FUNC, methodNode->sym->name, NULL, NULL);
}
```

Salta métodos `extern`.

## Funciones Auxiliares

**Temporales** (`newTemp()`):
```c
char* newTemp(TacCode* tac) {
    char* temp = malloc(16);
    sprintf(temp, "t%d", tac->tempCount++);
    return temp;
}
```

**Etiquetas** (`newLabel()`):
```c
char* newLabel(TacCode* tac) {
    char* label = malloc(16);
    sprintf(label, "L%d", tac->labelCount++);
    return label;
}
```

**Emisión** (`emitTAC()`):
```c
void emitTAC(TacCode* tac, TacOp op, char* dest, char* arg1, char* arg2)
```

Agrega instrucción al final de la lista enlazada (O(1)).

## Referencias

Implementación: `tac.h` y `tac.c`, Estructuras: [Estructura](/codinter/structure), Ejemplos: [Ejemplos](/codinter/examples)
