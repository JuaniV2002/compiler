---
title: "Ejemplos de Assembly"
description: "Traducciones completas de TAC a x86-64."
---

# Ejemplos de Traducción Assembly

## Ejemplo 1: Función Simple

**TAC**:
```
func suma
  t0 = a + b
  return t0
endfunc suma
```

**x86-64**:
```asm
suma:
    enter $(8 * 3), $0          ; a, b, t0
    mov %rdi, -16(%rbp)         ; Guardar param a
    mov %rsi, -24(%rbp)         ; Guardar param b
    mov -16(%rbp), %r10
    mov -24(%rbp), %r11
    add %r11, %r10
    mov %r10, -8(%rbp)          ; t0 = a + b
    mov -8(%rbp), %rax          ; Retorno
    leave
    ret
```

Stack: `-8(%rbp)=t0`, `-16(%rbp)=a`, `-24(%rbp)=b`

## Ejemplo 2: Programa con Llamada

**Código**:
```c
integer suma(integer a, integer b) {
    return a + b;
}
void main() {
    integer z = suma(5, 10);
}
```

**x86-64**:
```asm
suma:
    enter $(8 * 3), $0
    mov %rdi, -16(%rbp)
    mov %rsi, -24(%rbp)
    mov -16(%rbp), %r10
    mov -24(%rbp), %r11
    add %r11, %r10
    mov %r10, -8(%rbp)
    mov -8(%rbp), %rax
    leave
    ret

.globl main

main:
    enter $(8 * 2), $0          ; z, t1
    mov $5, %rdi                ; Param 1
    mov $10, %rsi               ; Param 2
    mov $0, %rax
    call suma
    mov %rax, -16(%rbp)         ; t1 = call suma
    mov -16(%rbp), %r10
    mov %r10, -8(%rbp)          ; z = t1
    mov $0, %rax
    leave
    ret
```

## Ejemplo 3: If-Else

**TAC**:
```
func main
  x = 5
  t0 = x < 10
  if !t0 goto L0
  y = 1
  goto L1
L0:
  y = 2
L1:
endfunc main
```

**x86-64**:
```asm
.globl main

main:
    enter $(8 * 3), $0
    mov $5, %r10
    mov %r10, -8(%rbp)          ; x = 5
    mov -8(%rbp), %r10
    mov $10, %r11
    cmp %r11, %r10
    mov $0, %r11
    mov $1, %r10
    cmovl %r10, %r11            ; t0 = x < 10
    mov %r11, -24(%rbp)
    mov -24(%rbp), %r10
    mov $1, %r11
    cmp %r10, %r11
    jne .L0                     ; if !t0 goto L0
    mov $1, %r10
    mov %r10, -16(%rbp)         ; y = 1
    jmp .L1
.L0:
    mov $2, %r10
    mov %r10, -16(%rbp)         ; y = 2
.L1:
    mov $0, %rax
    leave
    ret
```

## Ejemplo 4: While

**TAC**:
```
func main
  i = 0
L0:
  t0 = i < 10
  if !t0 goto L1
  t1 = i + 1
  i = t1
  goto L0
L1:
endfunc main
```

**x86-64**:
```asm
.globl main

main:
    enter $(8 * 3), $0
    mov $0, %r10
    mov %r10, -8(%rbp)          ; i = 0
.L0:
    mov -8(%rbp), %r10
    mov $10, %r11
    cmp %r11, %r10
    mov $0, %r11
    mov $1, %r10
    cmovl %r10, %r11
    mov %r11, -16(%rbp)         ; t0 = i < 10
    mov -16(%rbp), %r10
    mov $1, %r11
    cmp %r10, %r11
    jne .L1
    mov -8(%rbp), %r10
    mov $1, %r11
    add %r11, %r10
    mov %r10, -24(%rbp)         ; t1 = i + 1
    mov -24(%rbp), %r10
    mov %r10, -8(%rbp)          ; i = t1
    jmp .L0
.L1:
    mov $0, %rax
    leave
    ret
```

## Ejemplo 5: División y Módulo

**TAC**: `cociente = x / y`, `resto = x % y`

**x86-64**:
```asm
    mov -8(%rbp), %rax          ; x / y
    cqo
    mov -16(%rbp), %r11
    idiv %r11
    mov %rax, -24(%rbp)         ; cociente = rax
    
    mov -8(%rbp), %rax          ; x % y
    cqo
    mov -16(%rbp), %r11
    idiv %r11
    mov %rdx, -32(%rbp)         ; resto = rdx
```

`cqo` extiende signo. Cociente→`%rax`, resto→`%rdx`.

## Ejemplo 6: Expresión Compleja

**TAC**: `(3 + 5) * 2 - 8 / 4`

**x86-64**:
```asm
    mov $3, %r10
    mov $5, %r11
    add %r11, %r10
    mov %r10, -8(%rbp)          ; t0 = 3 + 5
    
    mov -8(%rbp), %r10
    mov $2, %r11
    imul %r11, %r10
    mov %r10, -16(%rbp)         ; t1 = t0 * 2
    
    mov $8, %rax
    cqo
    mov $4, %r11
    idiv %r11
    mov %rax, -24(%rbp)         ; t2 = 8 / 4
    
    mov -16(%rbp), %r10
    mov -24(%rbp), %r11
    sub %r11, %r10
    mov %r10, -32(%rbp)         ; t3 = t1 - t2
```

## Compilación

```bash
# Usar GCC para enlazar
gcc program.s -o program
./program
```

## Referencias

[Generación](/assembly/generation) · [Arquitectura](/assembly/architecture) · `assembly.c`
