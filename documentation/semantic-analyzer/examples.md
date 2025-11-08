---
title: "Ejemplos de Análisis Semántico"
description: "Casos de error y validación."
---

# Ejemplos de Análisis Semántico

## Programa Válido

```c
program {
    integer suma(integer a, integer b) {
        return a + b;
    }

    void main() {
        integer x = suma(5, 10);
    }
}
```
:::callout tip
Todos los checks pasan: tipos correctos, return presente, main existe.
:::

## Errores Comunes

### Tipos Incompatibles

```c
boolean b = true;
integer x = b + 1;  // ❌ BOOLEAN + INTEGER
```

### Variable No Declarada

```c
integer x = y + 1;  // ❌ 'y' no existe
```

`getSymbol()` no encuentra 'y' en ningún nivel.

### Fuera de Scope

```c
if (true) {
    integer temp = 100;
}
integer x = temp;  // ❌ temp destruido al salir del if
```

### Falta Return

```c
integer suma(integer a, integer b) {
    integer r = a + b;  // ❌ No hay return
}
```

### Return en If sin Else

```c
integer test(integer x) {
    if (x > 0) {
        return 1;  // ❌ Falta else con return
    }
}
```
:::callout info
Ambos bloques deben tener return.
:::

### Parámetros Incorrectos

```c
integer suma(integer a, integer b) { return a + b; }

integer x = suma(5);        // ❌ Faltan argumentos
integer y = suma(5, 10, 3); // ❌ Sobran argumentos
integer z = suma(true, 10); // ❌ Tipo incorrecto
```

### Redeclaración

```c
integer x = 5;
integer x = 10;  // ❌ Ya declarado en este scope
```

Shadowing en scopes diferentes sí está permitido.

### Return en Void

```c
void imprimir(integer x) {
    return x;  // ❌ void no retorna valor
}
```

### Tipo de Return Incorrecto

```c
boolean esPar(integer n) {
    return n % 2;  // ❌ Retorna INTEGER, no BOOLEAN
}
```

### Falta Main

```c
integer suma(integer a, integer b) {
    return a + b;
}
// ❌ No hay main
```

### Main Incorrecto

```c
integer main() {  // ❌ main debe ser void
    return 0;
}
```
