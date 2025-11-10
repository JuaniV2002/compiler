---
title: "Analizador Semántico"
description: "Verificación semántica del AST."
---

# Analizador Semántico

## Descripción General

El analizador semántico recibe el AST del parser y verifica que el programa tenga sentido más allá de la sintaxis correcta. Por ejemplo que las variables existan, los tipos sean compatibles, los métodos tengan return, etc.
Además construye la tabla de símbolos para llevar el registro de las variables y métodos que componen el programa.

**Entrada**: AST del parser  
**Salida**: Errores semánticos (si hay), o luz verde para seguir además del AST con los símbolos correspondientes y la tabla de símbolos obtenida.

## Archivos

```
semantic_analyzer.c/h   → Lógica de verificación
symbol.c/h              → Estructura de símbolos
ts.c/h                  → Tabla de símbolos con niveles
```

## Qué Verifica

### Estructura de Expresiones

#### Tipos Compatibles

```c
integer x = 5 + 10;     // ✓ INTEGER + INTEGER
bool b = x > 5;         // ✓ Comparación retorna BOOL
integer y = b + 1;      // ❌ No se puede sumar BOOL + INTEGER
```

#### Expresiones Completas

```c
integer x = 5 + 10;         // ✓ INTEGER + INTEGER
bool a = true && false;     // ✓ BOOL and BOOL

integer x = 5 + ;       // ❌ Falta de un término
integer y =  + 10;
bool a = true && ;
bool b =  && false;
```

### Variables Declaradas

```c
integer x = 5;
integer y = z + 1;  // ❌ 'z' no existe
```

### Scope Correcto

```c
void main() {
    integer temp = 0;
    if (true) {
        temp = 100;
    }
    integer x = temp;
}
```

### Scope Incorrecto

```c
void main() {
    if (true) {
        integer temp = 100;
    }
    integer x = temp;  // ❌ temp ya no está en scope
}
```

### Returns

```c
integer suma(integer a, integer b) {
    return a + b;  // ✓ Retorna INTEGER
}

integer resta(integer a, integer b) {
    integer r = a - b;  // ❌ Falta return
}

bool multiplicacion(integer a, integer b) {
    return a * b;  // ❌ Tipos incompatibles BOOL e INTEGER
}
```

### Llamadas a Métodos

```c
integer suma(integer a, integer b) { ... }

void main() {
    integer x = suma(5, 10);    // ✓ Se corresponden los parametros actuales con los formales
    integer y = suma(5);         // ❌ Faltan argumentos
    integer z = suma(true, 10);  // ❌ Tipo incorrecto
    integer w = resta(7, 2);    // ❌ Método inexistente
}
```

### Método Main

#### Método Correcto

```c
void main() {  // ✓ void, sin parámetros
    // ...
}
```

#### Método Incorrecto

```c
integer main(bool a, integer x) {  // ❌ integer, con parámetros
    // ...
}
```

#### Multiples Declaraciones

```c
void main() {  // ❌ debe existir un solo método main
    // ...
}

void main() {  // ❌ debe existir un solo método main
    // ...
}
```

### Sentecias de Control

#### if

```c
bool esPar(integer x) {
    if (x % 2 == 0) then {  // ✓ expresion BOOLEANA
        return true;
    }

    if (x % 2) then {   // ❌ expresion NO BOOLEANA
        return true;
    }

    return false;
}
```

#### while

```c
integer sumaHasta10(integer x) {
    while (x < 10) then {  // ✓ expresion BOOLEANA
        x = x + 1;
    }

    while (x + 3) then {   // ❌ expresion NO BOOLEANA
        x = x + 1;
    }

    return x;
}
```

## Funciones Principales

### findType()

Determina el tipo de una expresión recorriendo el AST:

```c
infoType findType(Node* root) {
    switch (root->t_Node) {
        case N_TERM:    // Literal o variable
            return root->sym->type;
        case N_PLUS:
        case N_AND:     // Operadores aritméticos, lógicos e invocaciones a métodos → buscar en hijos
            if (root->sym) return root->sym->type;
            return findType(root->left) o findType(root->right);
    }
}

```

### checkParameters()

Compara los argumentos con los parámetros del método:

```c
int checkParameters(Node* methodCall, Level* symbolTable) {
    // 1. Obtener lista de parámetros del método
    // 2. Recorrer argumentos de la llamada
    // 3. Verificar cantidad y tipos
}
```

## fullCheck()

Función que valida todo el AST:

```c
void fullCheck(Node* root, Level* symbolTable) {
    // 1. Recorre cada uno de los nodos del árbol
    // 2. Realiza el chequeo correspondiente de cada nodo
}
```

## analyzeSemantics()

Función principal del analizador:

```c
int analyzeSemantics(Node* root, Stack* stack) {
    // 1. Invoca a fullCheck()
    // 2. Verifica la existencia de un método 'main'
}
```

## Tabla de Símbolos

La tabla se organiza en **niveles** (como una pila):

```
  Nivel 0 (global) → símbolos de métodos y variables → ...
        ↓
  Nivel 1 (main) → símbolos de métodos y variables → ...
        ↓
Nivel 2 (bloque if) → símbolos de métodos y variables → ...
        ↓
       ...
        ↓
       NULL
```

Al buscar una variable, se empieza desde el nivel más interno hacia afuera.

:::callout info Operaciones
- `openNewLevel()`: Al entrar a un bloque (se encuentra en métodos, if/then/else y while)
- `closeLevel()`: Al salir de un bloque
- `insertSymbol()`: Agregar variable/método
- `getSymbol()`: Buscar símbolo
:::

## Siguientes Pasos

- **[Verificaciones](semantic-analyzer/checks)**: Detalles de cada verificación
- **[Tabla de Símbolos](semantic-analyzer/symbol-table)**: Estructura completa
- **[Ejemplos](semantic-analyzer/examples)**: Casos de error comunes

## División del Trabajo

<!-- TODO: Completar por el estudiante -->

## Problemas Conocidos

<!-- TODO: Completar por el estudiante -->