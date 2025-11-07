---
title: "Ejemplos de Parsing"
description: "Casos de uso del parser."
---

# Ejemplos de Parsing

## Programa Básico

### Código

```c
integer suma(integer a, integer b) {
    return a + b;
}

void main() {
    integer x = 5;
    integer y = suma(x, 10);
}
```

### AST Generado

```
PROGRAM
├── METHOD suma (returnType=INTEGER)
│   ├── PARAMS: a (INTEGER), b (INTEGER)
│   └── BODY
│       └── RETURN
│           └── + (N_PLUS)
│               ├── VAR a
│               └── VAR b
│
└── METHOD main (returnType=VOID)
    └── BODY
        ├── VAR_DECL x = LITERAL 5
        └── VAR_DECL y = CALL suma(x, 10)
```

## Expresiones con Precedencia

### Código

```c
integer resultado = 3 + 5 * 2;
```

### AST

```
ASSIGN
├── VAR resultado
└── + (N_PLUS)
    ├── LITERAL 3
    └── * (N_MULT)
        ├── LITERAL 5
        └── LITERAL 2
```

:::callout info Precedencia
El `*` se evalúa primero porque tiene mayor precedencia que `+`.
:::

## Control de Flujo

### If/Else

```c
if (x > 5) {
    return x;
} else {
    return 5;
}
```

**AST**:
```
IF
├── CONDITION: > (N_GREAT)
│   ├── VAR x
│   └── LITERAL 5
├── THEN
│   └── RETURN
│       └── VAR x
└── ELSE
    └── RETURN
        └── LITERAL 5
```

### While

```c
while (i < 10) {
    i = i + 1;
}
```

**AST**:
```
WHILE
├── CONDITION: < (N_LESS)
│   ├── VAR i
│   └── LITERAL 10
└── BODY
    └── ASSIGN
        ├── VAR i
        └── + (N_PLUS)
            ├── VAR i
            └── LITERAL 1
```

## Errores Comunes

### Falta Punto y Coma

```c
integer x = 5
integer y = 10;  // ❌
```

**Error**: `syntax error en línea 2`

### Paréntesis Desbalanceados

```c
if (x > 5 {  // ❌ Falta ')'
    return x;
}
```

**Error**: `syntax error, unexpected '{'`

### Expresión Incompleta

```c
integer z = ;  // ❌
```

**Error**: `syntax error, unexpected ';'`

## Visualización del AST

Para ver el AST de un programa, puedes usar `printAST()`:

```bash
$ ./compiler test.ctds
```

La salida usa colores para distinguir tipos de nodos (azul para declaraciones, verde para expresiones, amarillo para operadores, etc.).
