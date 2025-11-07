---
title: "Arquitectura x86-64"
description: "Registros, convenciones y ABI."
---

# Arquitectura x86-64

## Registros Usados

| Registro | Uso |
|:--------:|:----|
| `%rax` | Retornos, división |
| `%rcx` | 4º parámetro |
| `%rdx` | 3º parámetro, resto (módulo) |
| `%rsi` | 2º parámetro |
| `%rdi` | 1º parámetro |
| `%rbp` | Base pointer |
| `%rsp` | Stack pointer |
| `%r8` | 5º parámetro |
| `%r9` | 6º parámetro |
| `%r10` | **Temporal 1** |
| `%r11` | **Temporal 2** |

## Convención de Llamada

**Parámetros** (primeros 6):
```
%rdi, %rsi, %rdx, %rcx, %r8, %r9
```

**Retorno**: `%rax`

Limitación: Solo soportamos hasta 6 parámetros.

## Stack Frame

```
        Alta dirección
        +-----------------+
        | Dir. retorno    |
        +-----------------+
%rbp -> | %rbp anterior   |
        +-----------------+
-8      | Variable 1      |
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
mov $5, %r10           # Constante
mov -16(%rbp), %r10    # Desde stack
mov %r10, -16(%rbp)    # A stack
```

**Aritmética**:
```asm
add %r11, %r10
sub %r11, %r10
imul %r11, %r10        # Multiplicación con signo
```

**División**:
```asm
mov dividendo, %rax
cqo                    # Extender signo a rdx:rax
idiv divisor           # Cociente→rax, Resto→rdx
```

**Comparación**:
```asm
cmp %r11, %r10
cmovl %r10, %r11       # if r10 < r11
cmovg %r10, %r11       # if r10 > r11
cmove %r10, %r11       # if r10 == r11
```

**Saltos**:
```asm
jmp .L0                # Incondicional
je .L0                 # Si igual
jne .L0                # Si no igual
```

**Llamadas**:
```asm
call función
ret
```

## Ejemplo

**Código**: `integer suma(integer a, integer b) { return a + b; }`

**Assembly**:
```asm
suma:
    enter $(8 * 3), $0
    mov %rdi, -16(%rbp)      # Guardar a
    mov %rsi, -24(%rbp)      # Guardar b
    mov -16(%rbp), %r10
    mov -24(%rbp), %r11
    add %r11, %r10
    mov %r10, -8(%rbp)       # t0
    mov -8(%rbp), %rax       # Retorno
    leave
    ret
```

## Referencias

Guía: `x86-64-architecture-guide.md`
