---
title: "Compilador C-TDS"
description: "Documentación del compilador C-TDS."
---

# Compilador C-TDS

Este es un compilador completo para el lenguaje **C-TDS**, que traduce código fuente a ensamblador x86-64 optimizado.

## Pipeline del Compilador

:::steps
1. Código Fuente (.ctds)
2. [Scanner] → Tokens
3. [Parser] → AST
4. [Semantic Analyzer] → AST Validado
5. [TAC Generator] → Código Intermedio
6. [Optimizer] → TAC Optimizado
7. [Assembly Generator] → x86-64 (.ass)
:::

## Fases del Compilador

### 🔍 Parser

Analiza la sintaxis del código y construye el **AST (Abstract Syntax Tree)**. Usamos Bison con la gramática del lenguaje C-TDS. El AST se representa con nodos que tienen 3 punteros (`left`, `right`, `third`) para manejar estructuras como `if-else`.

**Ver**: [Documentación del Parser](compiler/parser)

### ✅ Semantic Analyzer

Valida que el código tenga sentido: tipos compatibles, variables declaradas, return en funciones, etc. Usa una **tabla de símbolos con niveles** para manejar scopes anidados.

**Verificaciones principales**:
- Tipos en operaciones y asignaciones
- Existencia de variables y funciones
- Parámetros correctos en llamadas
- Return obligatorio en funciones no-void

**Ver**: [Documentación del Analizador Semántico](compiler/semantic-analyzer)

### 📝 TAC (Código Intermedio)

Traduce el AST a **Three-Address Code**, una representación lineal más simple que facilita optimizaciones. Cada instrucción tiene máximo 3 direcciones: `dest = arg1 op arg2`.

**Características**:
- Temporales explícitos (`t0`, `t1`, ...)
- Etiquetas para control de flujo (`L0`, `L1`, ...)
- Independiente de la arquitectura

**Ver**: [Documentación de TAC](compiler/codinter)

### ⚡ Optimizaciones

Aplicamos optimizaciones sobre el TAC para mejorar rendimiento:

1. **Propagación de Constantes**: Evalúa `3 + 7` → `10` en tiempo de compilación
2. **Eliminación de Código Muerto**: Elimina variables no usadas y código inalcanzable

**Resultados**: Reducción promedio de **46.6%** en el código generado.

**Ver**: [Documentación de Optimizaciones](compiler/optimizations)

### 🖥️ Assembly x86-64

Genera código ensamblador siguiendo la convención **System V AMD64 ABI**.

**Decisiones de diseño**:
- Variables en stack (simplicidad > rendimiento)
- `%r10` y `%r11` como registros temporales
- Parámetros en registros: `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`

**Ver**: [Documentación de Assembly](compiler/assembly)

## Uso Rápido

```bash
# Compilar
make

# Ensamblar y enlazar
make run
```

## Estructura del Proyecto

```
compiler/
├── scanner.l              # Análisis léxico (Flex)
├── parser.y               # Análisis sintáctico (Bison)
├── ast.c/h                # Árbol de sintaxis abstracta
├── semantic_analyzer.c/h  # Análisis semántico
├── ts.c/h                 # Tabla de símbolos
├── tac.c/h                # Generación de TAC
├── optimizations.c/h      # Optimizaciones
├── assembly.c/h           # Generación de x86-64
└── main.c                 # Punto de entrada
```

## Navegación

Usa la barra lateral para explorar cada fase en detalle. Cada sección incluye:
- Explicación del algoritmo
- Estructuras de datos utilizadas
- Ejemplos con código real
- Referencias a la implementación

---

**Autores**: Santiago Pesce & Juan Ignacio Villanueva.
**Lenguaje**: C-TDS.
**Target**: Arquitecturas x86-64.
