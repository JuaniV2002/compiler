---
title: "Ejemplos de Parsing"
description: "Casos de uso del parser."
---

# Ejemplos de Parsing

## Programa Básico

### Código

```c
program {
    integer suma(integer a, integer b) {
        return a + b;
    }

    void main() {
        integer x = 5;
        integer y = suma(x, 10);
    }
}
```

### AST Generado

```
PROGRAM
├── METHOD_DECLARATION 'suma' -> int
│   ├── TERM 'a' (parámetro)
│   ├── TERM 'b' (parámetro)
│   └── BLOCK
│       └── RETURN
│           └── OPERATOR +
│               ├── TERM 'a'
│               └── TERM 'b'
└── METHOD_DECLARATION 'main' -> void
    └── BLOCK
        ├── VAR_DECLARATION 'x' (int) = 5
        └── VAR_DECLARATION 'y' (int) = 0
            └── ASSIGNMENT
                ├── TERM 'y'
                └── METHOD_CALL 'suma()'
                    ├── TERM 'x'
                    └── TERM 10
```

## Expresiones con Precedencia

### Código

```c
program {
    void main() {
        integer resultado = 3 + 5 * 2;
    }
}
```

### AST

```
PROGRAM
└── METHOD_DECLARATION 'main' -> void
    └── BLOCK
        └── VAR_DECLARATION 'resultado' (int) = 0
            └── ASSIGNMENT
                ├── TERM 'resultado'
                └── OPERATOR +
                    ├── TERM 3
                    └── OPERATOR *
                        ├── TERM 5
                        └── TERM 2
```

:::callout info Precedencia
El `*` se evalúa primero porque tiene mayor precedencia que `+`.
:::

## Control de Flujo

### If/Else

```c
program {
    integer test(integer x) {
        if (x > 5) {
            return x;
        } else {
            return 5;
        }
    }
}
```

**AST**:
```
PROGRAM
└── METHOD_DECLARATION 'test' -> int
    ├── TERM 'x' (parámetro)
    └── BLOCK
        └── IF
            ├── OPERATOR >
            │   ├── TERM 'x'
            │   └── TERM 5
            ├── THEN
            │   └── BLOCK
            │       └── RETURN
            │           └── TERM 'x'
            └── ELSE
                └── BLOCK
                    └── RETURN
                        └── TERM 5
```

### While

```c
program {
    void main() {
        integer i = 0;
        while (i < 10) {
            i = i + 1;
        }
    }
}
```

**AST**:

```
PROGRAM
└── METHOD_DECLARATION 'main' -> void
    └── BLOCK
        ├── VAR_DECLARATION 'i' (int) = 0
        └── WHILE
            ├── OPERATOR <
            │   ├── TERM 'i'
            │   └── TERM 10
            └── BLOCK
                └── ASSIGNMENT
                    ├── TERM 'i'
                    └── OPERATOR +
                        ├── TERM 'i'
                        └── TERM 1

```

## Errores Comunes

### Falta Punto y Coma

```c
integer x = 5
integer y = 10;  // ❌
```

**Error**: `ERROR en la línea 2: syntax error`[^1]

### Paréntesis Desbalanceados

```c
if (x > 5 {  // ❌ Falta ')'
    return x;
}
```

**Error**: `ERROR en la línea 1: syntax error`

### Expresión Incompleta

```c
integer z = ;  // ❌
```

**Error**: `ERROR en la línea 1: syntax error`

## Visualización del AST

Para ver el AST de un programa, compila el compilador y ejecuta:

```bash
$ ./c-tds -debug tests/<cualquier_programa_correcto>.ctds
```

:::callout warning
Aparte de mostrar el AST en la consola, el comando de arriba también mostrará otros mensajes de depuración (tabla de símbolos y código intermedio).
:::

La salida usa colores para distinguir tipos de nodos (azul para declaraciones, verde para expresiones, amarillo para operadores, etc.).

[^1]: El mensaje de error en todos los casos de ejemplo es el mismo. Durante la etapa del analizador semántico, se mostrarán mensajes más específicos.
