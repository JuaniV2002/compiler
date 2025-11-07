---
title: "Ejemplos y Resultados"
description: "Casos de uso y métricas."
---

# Ejemplos y Resultados

## Ejemplo 1: Propagación de Constantes

**Código**: `x = 3 + 7; y = x * 2;`

**Original** (5 instrucciones):
```
t0 = 3 + 7
x = t0
t1 = x * 2
y = t1
z = y
```

**Optimizado** (3 instrucciones):
```
x = 10
y = 20
z = 20
```

**Reducción**: 40%

## Ejemplo 2: Código Muerto

**Código**: `x = 5; y = 10; temp = 15; return; z = 20;`

**Original** (5 instrucciones):
```
x = 5
y = 10
temp = 15
return
z = 20
```

**Optimizado** (1 instrucción):
```
return
```

**Reducción**: 80%

## Ejemplo 3: Optimizaciones Combinadas

**Código**:
```c
integer suma(integer a, integer b) {
    return a + b;
}
void main() {
    integer temp1 = 3 + 4;
    integer temp2 = temp1 * 2;
    integer result = suma(5, 10);
}
```

**Original** (10 instrucciones):
```
func suma
  t0 = a + b
  return t0
endfunc suma

func main
  t1 = 3 + 4
  temp1 = t1
  t2 = temp1 * 2
  temp2 = t2
  param 5
  param 10
  t3 = call suma
  result = t3
endfunc main
```

**Tras Propagación** (8 instrucciones):
```
func suma
  t0 = a + b
  return t0
endfunc suma

func main
  temp1 = 7
  temp2 = 14
  param 5
  param 10
  t3 = call suma
  result = t3
endfunc main
```

**Tras Código Muerto** (6 instrucciones):
```
func suma
  t0 = a + b
  return t0
endfunc suma

func main
  param 5
  param 10
  t3 = call suma
  result = t3
endfunc main
```

**Reducción Total**: 40%

## Métricas Completas

| Test | Inicial | Final | Reducción |
|:-----|:-------:|:-----:|:---------:|
| constant_propagation | 17 | 14 | 17.6% |
| dead_code | 13 | 5 | 61.5% |
| opt_complete | 23 | 9 | 60.9% |

**Promedio**: **46.6%**

## Iteraciones

**Pasada 1**:
- Propagación: 15 cambios
- Código Muerto: 8 cambios

**Pasada 2**:
- Propagación: 3 cambios
- Código Muerto: 2 cambios

**Pasada 3**:
- Sin cambios → **Convergencia**

El optimizador itera hasta punto fijo (máximo 10 pasadas).

## Optimizaciones Futuras (TODO)

<!-- TODO: Implementar -->

**Strength Reduction**:
```
x * 2  →  x << 1
x / 2  →  x >> 1
```

**Common Subexpression Elimination**:
```
a = b + c
d = b + c    ← Duplicado
```
→
```
a = b + c
d = a
```

## Limitaciones

**No optimiza llamadas**:
```c
integer getNumber() { return 42; }
void main() {
    integer x = getNumber();  // No se evalúa
}
```

**No optimiza a través de loops**: Optimiza dentro de loops, no los desenrolla.

## Referencias

`optimizations.c` · Tests: `tests/test_*`
