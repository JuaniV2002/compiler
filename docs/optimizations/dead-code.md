---
title: "Eliminación de Código Muerto"
description: "Eliminación de instrucciones no utilizadas."
---

# Eliminación de Código Muerto

Elimina instrucciones cuyos resultados **nunca se usan** o son **inalcanzables**.

## Algoritmo (2 Pasadas)

```c
int optimizeDeadCode(TacCode* tac) {
    VarUsage* usageTable = NULL;
    
    // Pasada 1: Analizar uso de variables
    // - Marcar todas las asignaciones
    // - Marcar variables usadas (leídas)
    
    // Pasada 2: Eliminar instrucciones muertas
    // - Eliminar asignaciones a variables no usadas
    // - Eliminar código inalcanzable después de return/goto
    
    return changesCount;
}
```

## Tabla de Uso

```c
typedef struct VarUsage {
    char* varName;
    int isUsed;         // 1 si se lee
    int assignCount;    // Número de asignaciones
    struct VarUsage* next;
} VarUsage;
```

**Funciones**: `findOrCreateVarUsage()`, `markVariableAsUsed()`, `markVariableAsAssigned()`

## Ejemplo

**TAC Original**:
```
x = 5
y = 10
t0 = x + y
z = t0
```

**Pasada 1 - Análisis**:
- `x = 5`: Asignación → `{x: NO_USADO}`
- `y = 10`: Asignación → `{x: NO_USADO, y: NO_USADO}`
- `t0 = x + y`: Usa x, y; Asigna t0 → `{x: USADO, y: USADO, t0: NO_USADO}`
- `z = t0`: Usa t0; Asigna z → `{x: USADO, y: USADO, t0: USADO, z: NO_USADO}`

**Pasada 2 - Eliminación**:
- `z = t0`: z no se usa → **Eliminar**

**TAC Optimizado**:
```
x = 5
y = 10
t0 = x + y
```

## Código Inalcanzable

También elimina código después de `return`/`goto`:

```c
if (instr->op == TAC_RETURN || instr->op == TAC_GOTO) {
    // Eliminar instrucciones hasta siguiente label/endfunc
    // (si no tienen side effects)
}
```

**Ejemplo**:
```
return x
t0 = 5      ← Inalcanzable
```

**Optimizado**:
```
return x
```

## Efectos Secundarios

Operaciones con efectos secundarios **NO** se eliminan:

```c
static int hasSideEffect(TacInstr* instr) {
    switch (instr->op) {
        case TAC_CALL:
        case TAC_RETURN:
        case TAC_PARAM:
        case TAC_LABEL:
        case TAC_GOTO:
        case TAC_IF_FALSE:
        case TAC_BEGIN_FUNC:
        case TAC_END_FUNC:
            return 1;
        default:
            return 0;
    }
}
```

**Ejemplo**:
```
t0 = call func    ← Se mantiene (aunque t0 no se use)
x = 5             ← Se elimina si x no se usa
```

## Ejemplo Completo

**Original**:
```
x = 5
y = 10
temp = 15
return
z = 20
```

**Optimizado**:
```
return
```

- `x`, `y`, `temp`: No se usan → Eliminados
- `z`: Inalcanzable → Eliminado

**Reducción**: 80%

## Limitaciones

- Análisis simple (no rastrea punteros)
- Conservador con llamadas
- No detecta loops infinitos

## Referencias

`optimizations.c` (función `optimizeDeadCode`)
