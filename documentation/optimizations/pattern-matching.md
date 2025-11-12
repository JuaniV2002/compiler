---
title: "Búsqueda de Patrones"
description: "Simplificación de expresiones algebraicas."
---

# Búsqueda de Patrones

Busca y **reemplaza patrones de expresiones algebraicas** (multiplicaciones y divisiones por potencias de 2, operaciones con identidades algebraicas).

## Algoritmos

```c
int optimizeShifts(TacCode *tac) {
    if (!tac) return 0;

    int changesCount = 0;
    TacInstr* instr = tac->head;

    while (instr) {
        // 1. Solo procesar multiplicaciones y divisiones
        // 2. Verificar si arg2 es un número (constante)
        // 3. Para multiplicación (conmutativa), también verificar si arg1 es constante
        // 4. Verificar si el valor es una potencia de 2 positiva
        // 5. Calcular el exponente (número de bits a desplazar)

        // 6. Convertir la operación:
        //      x * 2^n  →  x << n
        //      2^n * x  →  x << n  (multiplicación conmutativa)
        //      x / 2^n  →  x >> n  (división solo con constante en arg2)

        // 7. Si la constante estaba en arg1 (multiplicación), reorganizar argumentos
        // 8. Reemplazar el operando constante con el exponente
    }

    return changesCount;
}

int optimizeAlgebraicIdentities(TacCode *tac) {
    if (!tac) return 0;
    
    int changesCount = 0;
    TacInstr* instr = tac->head;
    
    while (instr) {
        // 1. Verificar el tipo de operación (ADD, SUB, MUL, DIV)
        // 2. Para cada operación, identificar patrones de identidades algebraicas:
        //      Suma: x + 0 → x,  0 + x → x
        //      Resta: x - 0 → x,  0 - x → -x
        //      Multiplicación: x * 0 → 0,  0 * x → 0,  x * 1 → x,  1 * x → x
        //      División: 0 / x → 0,  x / 1 → x

        // 3. Si se detecta un patrón, simplificar la operación
        //      - A TAC_COPY para asignaciones directas o constantes
        //      - A TAC_NEG para negación (0 - x)
        // 4. Liberar memoria de argumentos innecesarios
        // 5. Actualizar argumentos según la simplificación
    }

    return changesCount;
}
```

## Ejemplos

### Shifts

**Original**:
```
x * 2;
z / 2;
y / 16;
z * 32;
2 * w;
8 * t;
x * 3;
```

**Optimizado**:
```
x << 1;
z >> 1;
y >> 4;
z << 5;
w << 1;  // 2 * w optimizado (constante en arg1)
t << 3;  // 8 * t optimizado (constante en arg1)
x * 3;   // se mantiene sin cambios (no es potencia de 2)
```

**Cambios**: 6 (4 shift izquierda + 2 shift derecha)

### Identidades Algebraicas

**Original**:
```
x + 0;
y - 0;
0 - z;
y * 0;
x * 1;
x / 1;
0 / x;
```

**Optimizado**:
```
x;
y;
-z;  // 0 - z optimizado a negación
0;
x;
x;
0;
```

**Cambios**: 7 (4 simplificaciones de variable + 2 reemplazos por 0 + 1 negación)

## Operaciones Aritméticas Soportadas

**'x' es una variable**
**'n' es un número entero**

`x + 0`, `0 + x`, `x - 0`, `0 - x`, `x * 2^n`, `2^n * x`, `x / 2^n`, `x * 0`, `0 * x`, `x * 1`, `1 * x`, `0 / x`, `x / 1`

## Limitaciones

### Shifts

- Solamente se aplican a potencias de 2
- Solo números positivos: No maneja potencias de 2 negativas
- No maneja expresiones complejas: Solo operaciones binarias simples, no expresiones anidadas
- División no conmutativa: Solo optimiza `x / 2^n`, no `2^n / x`

### Identidades Algebraicas

- Patrones limitados: Solo reconoce identidades básicas (suma/resta con 0, multiplicación con 0 y 1, división con 1, negación de 0)
- No optimiza patrones como: `x - x → 0`, `x / x → 1`, `x * x → x²`
- División por cero: Asume que `0 / x` es válido (no verifica que x ≠ 0)

### Comunes a Ambas

- No maneja arrays: Solo trabaja con escalares
- Solo forward propagation: No optimiza dentro de loops o con análisis hacia atrás
- Single-pass: Una sola pasada, no iteran hasta punto fijo
- No atraviesa funciones: Solo trabajan en el TAC local
- No verifica side-effects: Asumen que las operaciones son puras

## Referencias

`optimizations.c` (funciones `optimizeShifts' 'optimizeAlgebraicIdentities`)
