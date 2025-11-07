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
    char* name;
    int value;
    struct Symbol* nextParam;   // Para parámetros de métodos
} Symbol;

// ts.h
typedef struct TSNode {
    Symbol* symbol;
    struct TSNode* next;
} TSNode;

typedef struct Level {
    TSNode* tsNode;         // Lista de símbolos
    int levelNumber;        // 0 = global, 1+ = locales
    struct Level* nextLevel;
} Level;
```

**Organización**:
```
Level 0 → Level 1 → Level 2 → NULL
  ↓         ↓         ↓
TSNode    TSNode    TSNode
```

## Operaciones Principales

### initializeTS()

Crea nivel 0:

```c
Level* initializeTS() {
    Level* level = malloc(sizeof(Level));
    level->tsNode = NULL;
    level->levelNumber = 0;
    level->nextLevel = NULL;
    return level;
}
```

### openNewLevel() / closeLevel()

```c
// Al entrar a método/bloque
Level* openNewLevel(Level* firstLevel);

// Al salir
void closeLevel(Level* firstLevel);
```

:::callout warning
`closeLevel()` libera nodos pero no símbolos (están en el AST).
:::

### insertSymbol()

Agrega símbolo al nivel actual:

```c
Symbol* insertSymbol(Level* symbolTable, flagType flag,
                     infoType type, char* name, int value) {
    if (getSymbol(symbolTable, name)) {
        error("Símbolo ya existe");
        return NULL;
    }
    // Crear símbolo y TSNode
    // Agregar al final de la lista
}
```

### getSymbol()

Busca en un nivel (no recursivo):

```c
Symbol* getSymbol(Level* symbolTable, char* name) {
    TSNode* current = symbolTable->tsNode;
    while (current) {
        if (strcmp(current->symbol->name, name) == 0)
            return current->symbol;
        current = current->next;
    }
    return NULL;
}
```

## Ejemplo

```c
integer global = 100;
void main() {
    integer x = 5;
}
```
:::steps
1. `initializeTS()` → Nivel 0
2. `insertSymbol(nivel0, VAR, "global", 100)`
3. `openNewLevel()` → Nivel 1
4. `insertSymbol(nivel1, VAR, "x", 5)`
5. `closeLevel()` → Nivel 1 eliminado
:::