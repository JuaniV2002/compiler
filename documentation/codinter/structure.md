---
title: "Estructura del TAC"
description: "Formato y operaciones del código intermedio."
---

# Estructura del TAC

## Formato

Cada instrucción tiene máximo 3 "direcciones":

```
dest = arg1 op arg2
```

## Tipos de Instrucciones

**Aritméticas**:
```
t0 = a + b
t1 = a - b
t2 = a * b
t3 = a / b
t4 = a % b
```

**Relacionales** (resultado: 1 o 0):
```
t0 = a < b
t1 = a > b
t2 = a == b
```

**Lógicas**:
```
t0 = a && b
t1 = a || b
t2 = !a
t3 = -a  # Negación aritmética
```

**Asignación**:
```
x = 5    # TAC_COPY
```

**Control de flujo**:
```
L0:              # TAC_LABEL
goto L0          # TAC_GOTO
if !t0 goto L1   # TAC_IF_FALSE (con negación)
```

**Funciones**:
```
param x          # TAC_PARAM
t0 = call suma   # TAC_CALL
return t0        # TAC_RETURN
```

**Delimitadores**:
```
func nombre      # TAC_BEGIN_FUNC
endfunc nombre   # TAC_END_FUNC
```

## Temporales y Etiquetas

- **Temporales**: `t0, t1, t2, ...` (contador en `TacCode->tempCount`)
- **Etiquetas**: `L0, L1, L2, ...` (contador en `TacCode->labelCount`)

## Ejemplo

**Código**:
```c
integer suma(integer a, integer b) {
    return a + b;
}
void main() {
    integer y = suma(5, 10);
}
```

**TAC**:
```
func suma
  t0 = a + b
  return t0
endfunc suma

func main
  param 5
  param 10
  t1 = call suma
  y = t1
endfunc main
```

## Referencias

Implementación: `tac.h`, Generación: [Generación](/codinter/generation)
