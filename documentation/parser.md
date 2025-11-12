---
title: "Parser del Compilador C-TDS"
description: "Análisis sintáctico y construcción del AST."
---

# Parser y Scanner

## Descripción General

El parser toma los tokens del scanner y verifica que la sintaxis sea correcta según la gramática de C-TDS. Mientras analiza, construye el **AST (Árbol de Sintaxis Abstracta)** que representa la estructura del programa.

**Entrada**: Tokens del scanner  
**Salida**: AST + errores sintácticos (si hay)

## Scanner (Análisis Léxico)

Antes de que el parser pueda trabajar, el **scanner** (también llamado analizador léxico) procesa el código fuente y lo convierte en tokens.

### Implementación del Scanner

Usamos **Flex/Lex** para generar el scanner automáticamente a partir de expresiones regulares definidas en `scanner.l`.

**Archivo**: `scanner.l`

### Funcionalidad Principal

El scanner realiza las siguientes tareas:

1. **Reconocimiento de tokens**: Identifica palabras reservadas, identificadores, operadores, números y símbolos de agrupación
2. **Manejo de comentarios**: 
   - Comentarios de línea (`//`)
   - Comentarios de bloque (`/* */`) con detección de comentarios no cerrados
3. **Validación de literales**: Verifica que los números enteros estén en el rango válido (`INT_MIN` a `INT_MAX`)
4. **Eliminación de espacios en blanco**: Ignora espacios, tabs, saltos de línea
5. **Detección de errores léxicos**: Caracteres no válidos y números fuera de rango

### Tokens Reconocidos

- **Palabras reservadas**: `program`, `extern`, `void`, `integer`, `bool`, `true`, `false`, `if`, `then`, `else`, `while`, `return`
- **Operadores aritméticos**: `+`, `-`, `*`, `/`, `%`
- **Operadores relacionales**: `<`, `>`, `==`
- **Operadores lógicos**: `!`, `&&`, `||`
- **Operador de asignación**: `=`
- **Símbolos de agrupación**: `(`, `)`, `{`, `}`
- **Separadores**: `,`, `;`
- **Identificadores**: Comienzan con letra y pueden contener letras, dígitos y guiones bajos
- **Literales enteros**: Números enteros con signo opcional

:::callout info ¿Por qué Flex?
Flex genera código C optimizado para el análisis léxico. Define patrones con expresiones regulares y automáticamente genera el código que reconoce esos patrones en el texto fuente.
:::

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

EL trabajo se dividión de forma equitativa entre ambos integrantes del equipo, colaborando en la implementación del parser, scanner y la construcción del AST.

## Problemas Conocidos

Tuvimos problemas a la hora de armar los nodos del AST, en dónde en un solo símbolo se debían guardar diferentes tipos de datos. Esto se solucionó utilizando un solo tipo de nodo con múltiples punteros a hijos, y dependiendo del tipo de nodo, algunos punteros quedan en NULL.

En el parser en sí, utilizábamos una sola regla en la declaración de los métodos, entonces separamos las reglas para llevar el registro del método que estábamos agregando para luego sumarle los parámetros formales y el bloque de código. 

Algo similar ocurrió en las llamadas de los métodos, en dónde guardábamos un puntero apuntando a la llamada del método, y luego se iba armando el árbol con los parámetros actuales.

En el scanner, no hubieron problemas.

## Referencias

- Implementación: `parser.y`, `ast.c`, `ast.h`
- Herramienta: [GNU Bison](https://www.gnu.org/software/bison/)
- Libro: "Compilers: Principles, Techniques, and Tools" (Dragon Book), Cap. 4