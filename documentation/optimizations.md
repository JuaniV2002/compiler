---
title: "Optimizaciones"
description: "Optimizaciones aplicadas al TAC."
---

# Optimizaciones

Mejora la eficiencia del código TAC mediante transformaciones que preservan la semántica.

**Entrada**: TAC generado → **Salida**: TAC optimizado

## Estructuras

```c
typedef struct ConstValue {
    char* varName;
    int value;
    int isConstant;  // 1 si constante conocida
    struct ConstValue* next;
} ConstValue;

typedef struct VarUsage {
    char* varName;
    int isUsed;         // 1 si se lee
    int assignCount;    // Número de asignaciones
    struct VarUsage* next;
} VarUsage;
```

## Optimizaciones Implementadas

### 1. Propagación de Constantes

Reemplaza variables con valores conocidos y evalúa expresiones en tiempo de compilación (constant folding).

**Ejemplo**: `x = 3 + 7; y = x * 2;`

**Original**:
```
t0 = 3 + 7
x = t0
t1 = x * 2
y = t1
```

**Optimizado**:
```
x = 10
y = 20
```

### 2. Eliminación de Código Muerto

Elimina instrucciones cuyos resultados nunca se usan.

**Ejemplo**: `temp = 5; return 10; x = 20;`

**Original**:
```
temp = 5
return 10
x = 20
```

**Optimizado**:
```
return 10
```

### 3. Optimizaciones Futuras (TODO)

<!-- TODO: Implementar -->


## Función Principal

```c
int applyOptimizations(TacCode* tac, char* optName) {
    // optName: "constant-propagation", "dead-code", "all"
}
```

**Opciones**:
- `"constant-propagation"`: Solo propagación
- `"dead-code"`: Solo código muerto
- `"all"`: Ambas (iterativo)

## Métricas Reales

| Test | Inicial | Final | Reducción |
|:-----|:-------|:-----|:---------|
| constant_propagation | 17 | 14 | 17.6% |
| dead_code | 13 | 5 | 61.5% |
| opt_complete | 23 | 9 | 60.9% |

**Promedio**: 46.6% de reducción

## Siguientes Pasos

- **[Propagación de Constantes](/optimizations/constant-propagation)**: Algoritmo y constant folding
- **[Código Muerto](/optimizations/dead-code)**: Análisis de 2 pasadas
- **[Ejemplos](/optimizations/examples)**: Casos con métricas

## División del Trabajo

<!-- TODO: Completar por el estudiante -->

## Problemas Conocidos

<!-- TODO: Completar por el estudiante -->

## Referencias

Implementación: `optimizations.c`, `optimizations.h`
