---
title: "Código Intermedio (TAC)"
description: "Generación de Three-Address Code."
---

# Código Intermedio (TAC)

El generador traduce el AST validado a **TAC (Three-Address Code)**, una representación lineal de bajo nivel pero independiente de la arquitectura.

:::callout info ¿Por qué TAC?
Es el puente entre código de alto nivel y ensamblador: más simple que el AST, fácil de optimizar y portátil.
:::

**Entrada**: AST validado → **Salida**: Secuencia de instrucciones TAC

## Estructuras Principales

```c
typedef struct TacInstr {
    TacOp op;
    char* dest;
    char* arg1;
    char* arg2;
    struct TacInstr* next;
} TacInstr;

typedef struct TacCode {
    TacInstr* head;
    TacInstr* tail;
    int tempCount;
    int labelCount;
} TacCode;
```

## Operaciones

```c
typedef enum {
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV, TAC_MOD,   // Aritméticas
    TAC_LT, TAC_GT, TAC_EQ,                        // Relacionales
    TAC_AND, TAC_OR, TAC_NOT, TAC_NEG,             // Lógicas
    TAC_COPY,                                      // Asignación
    TAC_LABEL, TAC_GOTO, TAC_IF_FALSE,             // Control de flujo
    TAC_PARAM, TAC_CALL, TAC_RETURN,               // Funciones
    TAC_BEGIN_FUNC, TAC_END_FUNC                   // Delimitadores
} TacOp;
```

## Ejemplo

**Código**: `integer x = 3 + 5 * 2;`

**TAC**:
```
t0 = 5 * 2
t1 = 3 + t0
x = t1
```

## Características

- **Lineal**: Máximo 3 direcciones por instrucción (`dest = arg1 op arg2`)
- **Temporales explícitos**: `t0, t1, t2, ...` (generados con `newTemp()`)
- **Etiquetas**: `L0, L1, L2, ...` (generadas con `newLabel()`)

## Funciones Clave

- `initTAC()`: Inicializa estructura TAC
- `emitTAC()`: Agrega instrucción a la lista
- `generateExpr()`: Traduce expresiones recursivamente
- `generateStmt()`: Traduce sentencias (if, while, return, etc.)
- `generateMethod()`: Traduce métodos completos
- `printTAC()` / `writeTAC()`: Imprime código generado

## Siguientes Pasos

- **[Estructura](codinter/structure)**: Tipos de instrucciones
- **[Generación](codinter/generation)**: Algoritmos de traducción
- **[Ejemplos](codinter/examples)**: Casos completos

## División del Trabajo

- Juani: impementó la generación de código intermedio.
- Santi: se abocó en el desarrollo del analizador semántico, debido a retrasos en el desarrollo.

## Problemas Conocidos

No se detectaron problemas en esta fase.

## Referencias

Implementación: `tac.c`, `tac.h`
