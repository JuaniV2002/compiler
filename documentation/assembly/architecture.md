---
title: "Arquitectura x86-64"
description: "Registros, convenciones y ABI."
---

# Arquitectura x86-64

## Registros Usados

| Registro | Uso |
|:--------|:----|
| `%rax` | Retornos, división (cociente) |
| `%rdi` | 1º parámetro |
| `%rsi` | 2º parámetro |
| `%rdx` | 3º parámetro, módulo (resto) |
| `%rcx` | 4º parámetro |
| `%r8` | 5º parámetro |
| `%r9` | 6º parámetro |
| `%rbp` | Base pointer (frame pointer) |
| `%rsp` | Stack pointer |
| `%r10` | **Temporal 1** (operaciones) |
| `%r11` | **Temporal 2** (operaciones) |

## Convención de Llamada

**Parámetros** (primeros 6):
```
%rdi, %rsi, %rdx, %rcx, %r8, %r9
```

**Retorno**: `%rax`

:::callout warning
Limitación: Solo soportamos hasta 6 parámetros.
:::

## Stack Frame

```
        Alta dirección
        +-----------------+
        | Dir. retorno    |
        +-----------------+
%rbp -> | %rbp anterior   |
        +-----------------+
-8      | Variable 1      | ← Primera variable/temporal
-16     | Variable 2      |
-24     | Variable 3      |
        ...
%rsp -> | Espacio libre   |
        Baja dirección
```

**Prólogo**:
```asm
enter $(8 * N), $0    # push rbp, mov rsp rbp, sub
```

**Epílogo**:
```asm
leave                 # mov rbp rsp, pop rbp
ret
```

## Instrucciones Principales

**Movimiento**:
```asm
mov $5, %r10           # Constante a registro
mov -8(%rbp), %r10     # Desde stack a registro
mov %r10, -8(%rbp)     # De registro a stack
```

**Aritmética**:
```asm
add %r11, %r10         # r10 = r10 + r11
sub %r11, %r10         # r10 = r10 - r11
imul %r11, %r10        # r10 = r10 * r11
neg %r10               # r10 = -r10
```

**División y Módulo**:
```asm
mov dividendo, %rax
cqo                    # Extender signo de rax a rdx:rax
idiv divisor           # rax = cociente, rdx = resto
```

**Comparaciones** (menor que):
```asm
cmp %r11, %r10         # Comparar r10 con r11
mov $0, %r11           # Asumir falso
mov $1, %r10           # Valor verdadero
cmovl %r10, %r11       # if r10 < r11: r11 = 1
```

**Comparaciones** (mayor que):
```asm
cmp %r11, %r10         # Comparar r10 con r11
mov $0, %r11           # Asumir falso
mov $1, %r10           # Valor verdadero
cmovg %r10, %r11       # if r10 > r11: r11 = 1
```

**Comparaciones** (igual):
```asm
cmp %r10, %r11         # Comparar r11 con r10
mov $0, %r11           # Asumir falso
mov $1, %r10           # Valor verdadero
cmove %r10, %r11       # if r10 == r11: r11 = 1
```

**Operaciones Lógicas**:
```asm
and %r11, %r10         # AND bit a bit
or %r11, %r10          # OR bit a bit
test %r10, %r10        # Test para NOT
sete %r10b             # Set if equal (para NOT)
```

**Saltos Condicionales**:
```asm
cmp %r10, %r11         # Comparar
jne .L0                # Saltar si no igual (usado en if !cond)
```

**Saltos Incondicionales**:
```asm
jmp .L0                # Salto incondicional
```

**Llamadas a Funciones**:
```asm
mov $0, %rax           # Limpiar rax (para variádicas)
call función           # Llamar función
ret                    # Retornar
```

## Ejemplo

**Código**: `integer suma(integer a, integer b) { return a + b; }`

**TAC**:
```
┌─ func suma
  t0 = a + b
  return t0
└─ endfunc suma
```

**Assembly**:
```asm
suma:
    enter $(8 * 3), $0        # 3 variables (a, b, t0)
    mov %rdi, -8(%rbp)        # Guardar parámetro a
    mov %rsi, -16(%rbp)       # Guardar parámetro b
    mov -8(%rbp), %r10        # Cargar a en r10
    mov -16(%rbp), %r11       # Cargar b en r11
    add %r11, %r10            # r10 = a + b
    mov %r10, -24(%rbp)       # Guardar resultado en t0
    mov -24(%rbp), %rax       # Cargar t0 en rax (retorno)
    leave
    ret
```

## Referencias

Guía: `x86-64-architecture-guide.pdf`
