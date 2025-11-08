---
title: "Analizador Semántico"
description: "Verificación semántica del AST."
---

# Analizador Semántico

## Descripción General

El analizador semántico recibe el AST del parser y verifica que el programa tenga sentido más allá de la sintaxis correcta. Cosas como que las variables existan, los tipos sean compatibles, los métodos tengan return, etc.

**Entrada**: AST del parser  
**Salida**: Errores semánticos (si hay) o luz verde para seguir

## Archivos

```
semantic_analyzer.c/h   → Lógica de verificación
symbol.c/h              → Estructura de símbolos
ts.c/h                  → Tabla de símbolos con niveles
```

## Qué Verifica

### Tipos Compatibles

```c
integer x = 5 + 10;     // ✓ INTEGER + INTEGER
boolean b = x > 5;      // ✓ Comparación retorna BOOLEAN
integer y = b + 1;      // ❌ No se puede sumar BOOLEAN + INTEGER
```

### Variables Declaradas

```c
integer x = 5;
integer y = z + 1;  // ❌ 'z' no existe
```

### Scope Correcto

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
```

### Llamadas a Métodos

```c
integer suma(integer a, integer b) { ... }

void main() {
    integer x = suma(5, 10);    // ✓
    integer y = suma(5);         // ❌ Faltan argumentos
    integer z = suma(true, 10);  // ❌ Tipo incorrecto
}
```

### Método Main

```c
void main() {  // ✓ void, sin parámetros
    // ...
}
```

## Funciones Principales

### findType()

Determina el tipo de una expresión recorriendo el AST:

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

### checkReturn()

Verifica que los métodos tengan return en todos los caminos:

```c
int checkReturn(Node* root, Level* symbolTable) {
    // Recorre el AST buscando N_RETURN
    // Para if/else, verifica que ambos tengan return
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

## Tabla de Símbolos

La tabla se organiza en **niveles** (como una pila):

```
Nivel 0 (global) → Nivel 1 (main) → Nivel 2 (bloque if) → ...
```

Al buscar una variable, se empieza desde el nivel más interno hacia afuera.

:::callout info Operaciones
- `openNewLevel()`: Al entrar a un método/bloque
- `closeLevel()`: Al salir
- `insertSymbol()`: Agregar variable/método
- `getSymbol()`: Buscar símbolo
:::

## Siguientes Pasos

- **[Verificaciones](/semantic-analyzer/checks)**: Detalles de cada verificación
- **[Tabla de Símbolos](/semantic-analyzer/symbol-table)**: Estructura completa
- **[Ejemplos](/semantic-analyzer/examples)**: Casos de error comunes

## División del Trabajo

<!-- TODO: Completar por el estudiante -->

## Problemas Conocidos

<!-- TODO: Completar por el estudiante -->