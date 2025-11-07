---
title: "Parser del Compilador C-TDS"
description: "Análisis sintáctico y construcción del AST."
---

# Parser (Analizador Sintáctico)

## Descripción General

El parser toma los tokens del scanner y verifica que la sintaxis sea correcta según la gramática de C-TDS. Mientras analiza, construye el **AST (Árbol de Sintaxis Abstracta)** que representa la estructura del programa.

**Entrada**: Tokens del scanner  
**Salida**: AST + errores sintácticos (si hay)

## Implementación

Usamos **Bison/Yacc** para generar el parser automáticamente a partir de la gramática.

### Archivos

```
parser.y      → Gramática con reglas de producción
ast.h/ast.c   → Estructura y funciones del AST
```

:::callout info ¿Por qué Bison?
Simplifica el trabajo: solo declaras la gramática y las precedencias, y Bison genera todo el código del parser. Además detecta automáticamente ambigüedades en la gramática.
:::

## Decisiones de Diseño

### AST vs Parse Tree

Decidimos construir un AST en lugar de un parse tree completo porque:

- Elimina detalles innecesarios (paréntesis, comas, punto y coma)
- Más fácil de recorrer en las siguientes fases
- Ocupa menos memoria

**Ejemplo**: Para `x = 3 + 5;`, el parse tree incluiría los tokens `=` y `;`, pero el AST solo guarda:

```
   assign
   /    \
  x     add
       /   \
      3     5
```

### Construcción del AST

El AST se construye **mientras se parsea**, usando las acciones semánticas de Bison:

```yacc
expr: expr T_PLUS expr {
    $$ = newNode_NonTerminal(N_PLUS, NULL, $1, $3, NULL);
}
```

Cada vez que se reduce una regla, creamos el nodo correspondiente y lo enlazamos con sus hijos.

## Estructura del AST

### Tipos de Nodos

En `ast.h` definimos los tipos de nodos posibles:

```c
typedef enum { 
    N_PROG, N_VAR_DECL, N_METHOD_DECL, N_BLOCK, N_EXTERN,
    N_STATEMENT, N_ASSIGN, N_METHOD_CALL, N_EXPR,
    N_PLUS, N_MINUS, N_MULT, N_DIV, N_MOD,
    N_LESS, N_GREAT, N_EQUAL, N_AND, N_OR, N_NOT, N_NEG,
    N_IF, N_THEN, N_ELSE, N_WHILE, N_RETURN, N_TERM
} nodeType;
```

### Estructura de Nodo

Cada nodo tiene:

```c
typedef struct Node {
    nodeType t_Node;        // Tipo de nodo
    Symbol* sym;            // Símbolo asociado (si aplica)
    struct Node* left;      // Hijo izquierdo
    struct Node* right;     // Hijo derecho
    struct Node* third;     // Tercer hijo (para if/else, etc.)
} Node;
```

:::callout tip Flexibilidad
Usamos un solo tipo de nodo con 3 punteros para hijos. Dependiendo del tipo de nodo, algunos punteros quedan en NULL.
:::

## Manejo de Errores

Bison detecta automáticamente errores sintácticos y llama a `yyerror()`:

```c
void yyerror(const char *s) {
    fprintf(stderr, "ERROR en la linea %d: %s\n", yylineno, s);
}
```

Los mensajes son genéricos ("syntax error"), pero son suficientes para identificar dónde está el problema.

## Siguientes Pasos

- **[Gramática](/parser/grammar)**: Reglas de producción y precedencias
- **[AST](/parser/ast)**: Detalles de la estructura del árbol
- **[Ejemplos](/parser/examples)**: Casos de prueba

## División del Trabajo

<!-- TODO: Completar por el estudiante -->

## Problemas Conocidos

<!-- TODO: Completar por el estudiante -->

## Referencias

- Implementación: `parser.y`, `ast.c`, `ast.h`
- Herramienta: [GNU Bison](https://www.gnu.org/software/bison/)
- Libro: "Compilers: Principles, Techniques, and Tools" (Dragon Book), Cap. 4