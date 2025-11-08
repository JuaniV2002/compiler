---
title: "Abstract Syntax Tree (AST)"
description: "Estructura del árbol de sintaxis abstracta."
---

# Abstract Syntax Tree (AST)

## Estructura del Nodo

En `ast.h` definimos un nodo genérico que sirve para todo el árbol:

```c
typedef struct Node {
    nodeType t_Node;        // Tipo de nodo
    Symbol* sym;            // Símbolo asociado (variable, constante, método)
    struct Node* left;      // Primer hijo
    struct Node* right;     // Segundo hijo
    struct Node* third;     // Tercer hijo (para if/else principalmente)
} Node;
```

### Tipos de Nodos

```c
typedef enum { 
    N_PROG,           // Raíz del programa
    N_VAR_DECL,       // Declaración de variable
    N_METHOD_DECL,    // Declaración de método
    N_BLOCK,          // Bloque { }
    N_EXTERN,         // Método externo
    N_STATEMENT,      // Sentencia genérica
    N_ASSIGN,         // Asignación
    N_METHOD_CALL,    // Llamada a método
    N_EXPR,           // Expresión
    // Operadores
    N_PLUS, N_MINUS, N_MULT, N_DIV, N_MOD,
    N_LESS, N_GREAT, N_EQUAL,
    N_AND, N_OR, N_NOT, N_NEG,
    // Control de flujo
    N_IF, N_THEN, N_ELSE, N_WHILE, N_RETURN,
    N_TERM            // Terminal (literal o variable)
} nodeType;
```

## Construcción del AST

### Funciones Principales

En `ast.c` tenemos dos funciones para crear nodos:

```c
// Para nodos hoja (literales, variables)
Node* newNode_Terminal(Symbol* symbol);

// Para nodos internos (operadores, sentencias)
Node* newNode_NonTerminal(nodeType type, Symbol* symbol, 
                          Node* left, Node* right, Node* third);
```

### Ejemplo de Construcción

En `parser.y`, cada regla crea el nodo correspondiente:

```yacc
EXPR : EXPR T_PLUS EXPR {
    $$ = newNode_NonTerminal(N_PLUS, NULL, $1, $3, NULL);
}
```

Esto construye:
```
    +
   / \
  $1  $3
```

## Recorrido del AST

### Función printAST

En `ast.c` implementamos `printAST()` que recorre el árbol recursivamente e imprime su estructura:

```c
void printAST(Node* root, int level);
```

La función usa colores ANSI para distinguir tipos de nodos:
- Azul: Declaraciones (métodos, variables)
- Verde: Expresiones
- Amarillo: Sentencias y operadores
- Morado: Terminales

### Ejemplo de Salida

Para `x = 3 + 5;`:

```
PROGRAM
└── METHOD_DECLARATION 'main' -> void
    └── BLOCK
        ├── VAR_DECLARATION 'x' (int)
        └── ASSIGNMENT
            ├── TERM 'x'
            └── OPERATOR +
                ├── TERM 3
                └── TERM 5
```

## Liberación de Memoria

La función `freeAST()` libera recursivamente todos los nodos:

```c
void freeAST(Node* root);
```

:::callout warning Importante
El `freeAST()` lleva un registro de símbolos liberados para evitar doble liberación (varios nodos pueden compartir el mismo símbolo).
:::

## Uso de los 3 Punteros

Dependiendo del tipo de nodo, los punteros se usan diferente:

- **Operadores binarios**: `left` y `right` son los operandos
- **If/else**: `left` = condición, `right` = then, `third` = else
- **While**: `left` = condición, `right` = cuerpo

:::callout tip Flexibilidad
Esta estructura genérica simplifica el código, aunque algunos punteros queden en NULL según el tipo de nodo.
:::
