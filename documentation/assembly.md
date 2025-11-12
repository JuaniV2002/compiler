---
title: "Generación de Código Assembly"
description: "Traducción de TAC a x86-64."
---

# Generación de Código Assembly x86-64

La fase final traduce TAC optimizado a ensamblador **x86-64** (System V AMD64 ABI para Linux).

**Entrada**: TAC → **Salida**: Archivo `.ass` (x86-64)

## Estructuras

```c
typedef struct VarInfo {
    char* name;
    int offset;              // Offset desde %rbp
    struct VarInfo* next;
} VarInfo;

typedef struct FuncContext {
    char* funcName;
    int stackSize;           // Tamaño en qwords
    VarInfo* vars;           // Lista de variables
    ParamStack paramStack;   // Hasta 6 params
} FuncContext;
```

## Decisiones de Diseño

**Variables en stack**: Todas las variables y temporales viven en el stack (simplicidad > rendimiento).

```asm
-8(%rbp)     # Variable 1
-16(%rbp)    # Variable 2
-24(%rbp)    # Variable 3
```

**Registros temporales**:
- `%r10`: Primer operando/resultado
- `%r11`: Segundo operando
- `%rax`: Retornos, división
- `%rdx`: Resto (módulo)

**Parámetros** (primeros 6):
```
%rdi, %rsi, %rdx, %rcx, %r8, %r9
```

## Ejemplo de Traducción

**TAC**: `t0 = a + b`

**Assembly**:
```asm
mov -8(%rbp), %r10     # Cargar a
mov -16(%rbp), %r11    # Cargar b
add %r11, %r10         # a + b
mov %r10, -24(%rbp)    # Guardar en t0
```

## Stack Frame

```
%rbp -> | RBP anterior  |
-8      | Variable 1    |
-16     | Variable 2    |
%rsp -> | ...           |
```

## Funciones Clave

- `newFuncContext()`: Inicializa contexto de función
- `addVar()`: Agrega variable al contexto, calcula offset
- `getVarOffset()`: Obtiene offset de variable
- `collectVarsAndParams()`: Escanea TAC para encontrar variables y parámetros
- `emitFunction()`: Genera código completo de función
- `generateAssembly()`: Punto de entrada, genera código para todas las funciones

## Siguientes Pasos

- **[Arquitectura](/assembly/architecture)**: Registros e instrucciones x86-64
- **[Generación](/assembly/generation)**: Algoritmos de traducción
- **[Ejemplos](/assembly/examples)**: Traducciones completas

## División del Trabajo

- Juani: implementó la generación de código assembly.
- Santi: corrigió bugs en el analizador semántico.

## Problemas Conocidos

No se detectaron problemas en esta fase.

## Referencias

Implementación: `assembly.c`, `assembly.h`
