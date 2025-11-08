---
title: "Ejemplos de TAC"
description: "Traducción completa de programas."
---

# Ejemplos de Traducción TAC

## Ejemplo 1: Función Simple

**Código**:
```c
program {
  integer suma(integer a, integer b) {
    return a + b;
  }
  void main() {
    integer y;
    y = suma(5, 10);
  }
}
```

**TAC**:
```
┌─ func suma
  t0 = a + b
  return t0
└─ endfunc suma

┌─ func main
  param 5
  param 10
  t1 = call suma
  y = t1
└─ endfunc main
```

## Ejemplo 2: If-Else

**Código**:
```c
program {
  void main() {
    integer x = 5;
    if (x < 10) {
        x = x + 1;
    } else {
        x = x - 1;
    }
  }
}
```

**TAC**:
```
┌─ func main
  x = 5
  t0 = x < 10
  if !t0 goto L0
  t1 = x + 1
  x = t1
  goto L1
  L0:
  t2 = x - 1
  x = t2
  L1:
└─ endfunc main
```

:::callout info
`if !t0` salta al else si la condición es falsa.
:::

## Ejemplo 3: While

**Código**:
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

**TAC**:
```
┌─ func main
  i = 0
  L0:
  t0 = i < 10
  if !t0 goto L1
  t1 = i + 1
  i = t1
  goto L0
  L1:
└─ endfunc main
```

## Ejemplo 4: Expresión Compleja

**Código**: 

```c
program {
  void main() {
      integer resultado = (3 + 5) * 2 - 8 / 4;
  }
}
```

**TAC**:
```
┌─ func main
  t0 = 3 + 5
  t1 = t0 * 2
  t2 = 8 / 4
  t3 = t1 - t2
  resultado = t3
└─ endfunc main
```

## Ejemplo 5: Factorial Recursivo

**Código**:
```c
program {
  integer factorial(integer n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
  }
}
```

**TAC**:
```
┌─ func factorial
  t0 = n <= 1
  if !t0 goto L0
  return 1
  goto L1
  L0:
  t1 = n - 1
  param t1
  t2 = call factorial
  t3 = n * t2
  return t3
  L1:
└─ endfunc factorial
```

## Ejemplo 6: Expresiones Lógicas

**Código**:

```c
program {
  void main() {
      boolean a;
      boolean b;
      boolean resultado;
      resultado = (a && b) || !a;
  }
}
```

**TAC**:
```
┌─ func main
  t0 = a && b
  t1 = !a
  t2 = t0 || t1
  resultado = t2
└─ endfunc main
```

## Ejemplo 7: Llamadas Anidadas

**Código**:

```c
program {
  integer multiplica(integer x, integer y) {
      return x * y;
  }
  
  integer suma(integer a, integer b) {
      return a + b;
  }
  
  void main() {
      integer resultado;
      resultado = suma(multiplica(2, 3), 5);
  }
}
```

**TAC**:
```
┌─ func multiplica
  t0 = x * y
  return t0
└─ endfunc multiplica

┌─ func suma
  t1 = a + b
  return t1
└─ endfunc suma

┌─ func main
  param 2
  param 3
  t2 = call multiplica
  param t2
  param 5
  t3 = call suma
  resultado = t3
└─ endfunc main
```

:::callout info
Los argumentos se evalúan antes de la llamada.
:::

## Comparación: AST vs TAC

**Código**: `x = 3 + 5 * 2`

**AST** (jerárquico):
```
ASSIGNMENT
├── TERM 'x'
└── OPERATOR +
    ├── TERM 3
    └── OPERATOR *
        ├── TERM 5
        └── TERM 2
```

**TAC** (lineal):
```
t0 = 5 * 2
t1 = 3 + t0
x = t1
```

:::callout tip Ventajas
- Secuencia lineal
- Temporales explícitos
- Fácil de optimizar.
:::

## Referencias

[Generación](/codinter/generation) · [Estructura](/codinter/structure) · `tac.c`
