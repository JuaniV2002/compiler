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

### 3. Búsqueda de Patrones

Busca y reemplaza patrones de expresiones algebraicas (multiplicaciones y divisiones por potencias de 2, operaciones con identidades algebraicas).

**Ejemplo**: `x * 2; z / 2; y / 16; z * 32; x * 3;      x + 0; y - 0; y * 0; x * 1; x / 1; 0 / x;`

**Original**:
```
x * 2;
z / 2;
y / 16;
z * 32;
x * 3;

x + 0;
y - 0;
y * 0;
x * 1;
x / 1;
0 / x;
```

**Optimizado**:
```
x << 1;
z >> 1;
y >> 4;
z << 5;
x * 3;  // se mantiene sin cambios (no es potencia de 2)

x;
y;
0;
x;
x;
0;
```


## Función Principal

```c
int applyOptimizations(TacCode* tac, char* optName) {
    // optName: "constant-propagation", "dead-code", "pattern-matching", "all"
}
```

**Opciones**:
- `"constant-propagation"`: Solo propagación
- `"dead-code"`: Solo código muerto
- `"pattern-matching"`: Solo búqueda de patrones
- `"all"`: Todas (iterativo)

## Métricas Reales

| Test | Inicial | Final | Reducción |
|:-----|:-------|:-----|:---------|
| constant_propagation | 17 | 14 | 17.6% |
| dead_code | 13 | 5 | 61.5% |
| pattern_matching | 18 | 11 | 38.89% |
| opt_complete | 19 | 5 | 73.68% |

**Promedio**: 47.92% de reducción

## Siguientes Pasos

- **[Propagación de Constantes](optimizations/constant-propagation)**: Algoritmo y constant folding
- **[Código Muerto](optimizations/dead-code)**: Análisis de 2 pasadas
- **[Código Muerto](optimizations/pattern-matching)**: Análisis de 2 pasadas
- **[Ejemplos](optimizations/examples)**: Casos con métricas

## División del Trabajo

- Juani: se dedicó a implementar las optimizaciones de propagación de constantes y eliminación de código muerto. Actualizando concurrentemente la documentación relacionada.
- Santi: se encargó de la optimización por búsqueda de patrones y de la creación de la documentación correspondiente.

## Problemas Conocidos

No se detectaron problemas en esta fase.

## Referencias

Implementación: `optimizations.c`, `optimizations.h`
