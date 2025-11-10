---
title: "Tabla de Símbolos"
description: "Gestión de scopes y símbolos."
---

# Tabla de Símbolos

## Estructuras

```c
// symbol.h
typedef struct Symbol {
    flagType flag;              // METH, VAR, PARAMET, CONST
    infoType type;              // TYPE_VOID, TYPE_INTEGER, TYPE_BOOL
    char* name;     // Solo para un símbolo con flag distinto de CONST
    int value;      // Valor de variables, parámetros y constantes o valor de retorno para métodos

    struct Symbol* nextParam;  // Lista de parámetros de los métodos
} Symbol;

// ts.h
typedef struct TSNode {
    Symbol* symbol;

    struct TSNode* next;
} TSNode;

typedef struct Level {
    struct TSNode* tsNode;  // Lista de símbolos

    int levelNumber;    // 0 = global, 1+ = locales
    struct Level* nextLevel;
} Level;

typedef struct Stack {
    struct Level* top;  // Tope de la pila
} Stack;
```

**Organización**:
```
  Nivel 0 (global) → TSNode → ...
        ↓
  Nivel 1 (main) → TSNode → ...
        ↓
Nivel 2 (bloque if) → TSNode → ...
        ↓
       ...
        ↓
       NULL
```

## Operaciones Principales

### initializeTS()

Crea nivel 0:

```c
Stack* initializeTS() {
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    Level* levelNode = (Level*) malloc(sizeof(Level));
    
    stack->top = levelNode;
    levelNode->tsNode = NULL;
    levelNode->levelNumber = 0;
    levelNode->nextLevel = NULL;
    return stack;
}
```

### openNewLevel() / closeLevel()

```c
// Al entrar a método/bloque
void openNewLevel(Stack* stack);

// Al salir
void closeLevel(Stack* stack);
```

:::callout warning
`closeLevel()` libera nodos pero no símbolos (están en el AST).
:::

### insertSymbol()

Agrega símbolo al nivel actual:

```c
Symbol* insertSymbol(Stack* stack, flagType flag, infoType type, char* name, int value) {
    
    // Crear nuevo símbolo y TSNode
    Symbol* newSym = newSymbol(flag, type, name, value);
    TSNode* newTSNode = (TSNode*) malloc(sizeof(TSNode));

    // Agregar al final de la lista
}
```

### insertParameter()

Agrega parámetro a un método dado:

```c
Symbol* insertParameter(Symbol* method, infoType type, char* name, int value) {
    // Crea e inserta el parámetro al final de la lista del método
    Symbol* newParam = newParameter(method, type, name, value);
    return newParam;
}
```

### getSymbol()

Busca en todos los niveles (no recursivo). Retorna la primer ocurrencia:

```c
Symbol* getSymbol(Stack* stack, char* name) {
    Level* currentLevel = stack->top;
    Symbol* fouded = NULL;

    // Buscar en todos los niveles de la tabla de símbolos

    return fouded;  // Si no se encontró, retorna NULL. Si se encontró, retorna el símbolo
}
```

## Ejemplo

```c
integer global = 100;

integer inc(integer x) {
    return x + 1;
}

void main() {
    integer x = 5;
    ineger y = inc(x);
}
```
:::steps
1. `Stack* stack = initializeTS()` → Nivel 0
2. `insertSymbol(stack, VAR, TYPE_INTEGER, "global", 100)` → inserta en Nivel 0
3. `insertSymbol(stack, METH, TYPE_INTEGER, "inc", 0)` → inserta en Nivel 0 (también inserta el parámetro 'x')
4. `openNewLevel()` → Nivel 1 (método 'inc')
5. `closeLevel()` → Nivel 1 eliminado
3. `openNewLevel()` → Nivel 1 (método 'main')
4. `insertSymbol(stack, VAR, TYPE_INTEGER, "x", 5)` → inserta en Nivel 1
5. `closeLevel()` → Nivel 1 eliminado
:::