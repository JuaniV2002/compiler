---
title: "Propagación de Constantes"
description: "Evaluación en tiempo de compilación."
---

# Propagación de Constantes

Reemplaza variables con valores conocidos y **evalúa expresiones en compilación** (constant folding).

## Algoritmo

```c
int optimizeConstantPropagation(TacCode* tac) {
    ConstValue* constTable = NULL;
    int changesCount = 0;
    
    for (TacInstr* instr = tac->head; instr; instr = instr->next) {
        // 1. Reemplazar operandos con constantes
        // 2. Evaluar si ambos operandos son constantes (folding)
        // 3. Actualizar tabla de constantes
    }
    
    return changesCount;
}
```

Itera sobre el TAC, mantiene tabla de constantes conocidas, reemplaza y evalúa.

## Tabla de Constantes

```c
typedef struct ConstValue {
    char* varName;
    int value;
    int isConstant;
    struct ConstValue* next;
} ConstValue;
```

**Operaciones**: `addConstValue()`, `findConstValue()`, `invalidateConstValue()`

**Ejemplo**:
```
x = 5       → Tabla: {x: 5}
y = x + 2   → Reemplaza x → 5, evalúa 5 + 2 → 7, Tabla: {x: 5, y: 7}
x = z       → Invalida x (ya no constante)
```

## Constant Folding

Evalúa operaciones con constantes:

```c
int foldBinaryOp(TacOp op, int left, int right, int* result) {
    switch (op) {
        case TAC_ADD:  *result = left + right; return 1;
        case TAC_SUB:  *result = left - right; return 1;
        case TAC_MUL:  *result = left * right; return 1;
        case TAC_DIV:  *result = (right != 0) ? left / right : 0; return 1;
        case TAC_MOD:  *result = (right != 0) ? left % right : 0; return 1;
        case TAC_LT:   *result = (left < right) ? 1 : 0; return 1;
        case TAC_GT:   *result = (left > right) ? 1 : 0; return 1;
        case TAC_EQ:   *result = (left == right) ? 1 : 0; return 1;
        // TAC_AND, TAC_OR también soportados
    }
}
```

Soporta aritméticas, relacionales y lógicas.

## Ejemplo

**TAC Original**:
```
x = 3
y = 7
t0 = x + y
z = t0 * 2
```

**Proceso**:
1. `x = 3` → Tabla: `{x: 3}`
2. `y = 7` → Tabla: `{x: 3, y: 7}`
3. `t0 = x + y` → Reemplaza: `t0 = 3 + 7` → Evalúa: `t0 = 10` → Tabla: `{x: 3, y: 7, t0: 10}`
4. `z = t0 * 2` → Reemplaza: `z = 10 * 2` → Evalúa: `z = 20`

**TAC Optimizado**:
```
x = 3
y = 7
t0 = 10
z = 20
```

**Cambios**: 4 (2 reemplazos + 2 foldings)

## Operaciones Soportadas

- **Aritméticas**: `+`, `-`, `*`, `/`, `%`
- **Relacionales**: `<`, `>`, `==`
- **Lógicas**: `&&`, `||`, `!`, `-` (negación)

## Limitaciones

- No atraviesa funciones
- No maneja arrays
- Solo forward propagation (no en loops)

## Referencias

`optimizations.c` (función `optimizeConstantPropagation`)
