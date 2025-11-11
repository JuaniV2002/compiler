---
title: "Ejemplos de Análisis Semántico"
description: "Casos de error y validación."
---

# Ejemplos de Análisis Semántico

## Programa Válido

```c
program {
    bool a = true;
    integer y = 12;

    integer suma(integer a, integer b) {
        return a + b;
    }

    integer restaHasta0(integer x) {
        while (x > 0) {
            x = x - 1;
        }

        return x;
    }

    void main() {
        integer x = suma(5, 10);

        if (!a) then {
            y = restaHasta0(y);
        } else {
            y = suma(x, y);
        }
    }
}
```
:::callout tip
Todos los checks pasan: tipos correctos, returns presentes, main existe.
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

### Expresiones Incompletas

```c
integer x = 5 + ;       // ❌ Falta de un término
integer y =  + 10;
bool a = true && ;
bool b =  && false;
```

### Fuera de Scope

```c
void main() {
    if (true) {
        integer temp = 100;
    }
    integer x = temp;  // ❌ temp destruido al salir del if
}
```

### Falta Return

```c
integer suma(integer a, integer b) {
    integer r = a + b;  // ❌ No hay return
}
```

### Parámetros Incorrectos

```c
integer suma(integer a, integer b) { return a + b; }

integer x = suma(5);        // ❌ Faltan argumentos
integer y = suma(5, 10, 3); // ❌ Sobran argumentos
integer z = suma(true, 10); // ❌ Tipo incorrecto
integer w = resta(7, 2);    // ❌ Método inexistente
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

### Sentecias de Control

#### if

```c
bool esPar(integer x) {
    if (x % 2) then {   // ❌ expresion NO BOOLEANA
        return true;
    }

    return false;
}
```

#### while

```c
integer sumaHasta10(integer x) {
    while (x + 10) {   // ❌ expresion NO BOOLEANA
        x = x + 1;
    }

    return x;
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

```c
void main(bool a, integer x) {  // ❌ main con parámetros
    // ...
}
```

#### Multiples Declaraciones

```c
void main() {
    // ...
}

void main() {  // ❌ debe existir un solo método main
    // ...
}
```