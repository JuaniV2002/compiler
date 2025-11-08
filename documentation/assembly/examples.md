---
title: "Ejemplos de Assembly"
description: "Traducciones completas de TAC a x86-64."
---

# Ejemplos de Traducción Assembly

## Ejemplo 1: Función Simple

**TAC**:
```
┌─ func suma
  t0 = a + b
  return t0
└─ endfunc suma
```

**x86-64**:
```asm
suma:
    enter $(8 * 3), $0          ; 3 variables: t0, b, a
    mov %rdi, -24(%rbp)         ; Guardar param a (1er param, último offset)
    mov %rsi, -16(%rbp)         ; Guardar param b (2do param)
    mov -24(%rbp), %r10         ; Cargar a
    mov -16(%rbp), %r11         ; Cargar b
    add %r11, %r10              ; a + b
    mov %r10, -8(%rbp)          ; Guardar t0 (primera var agregada)
    mov -8(%rbp), %rax          ; Cargar t0 para retorno
    leave
    ret
```

Stack: `-8(%rbp)=t0`, `-16(%rbp)=b`, `-24(%rbp)=a`

## Ejemplo 2: Programa con Llamada

**Código**:
```c
program {
  integer suma(integer a, integer b) {
      return a + b;
  }

  void main() {
      suma(5, 10);
  }
}
```

**x86-64**:
```asm
suma:
    enter   $(8 * 3), $0
    mov     %rdi, -16(%rbp)
    mov     -16(%rbp), %r10
    mov     -24(%rbp), %r11
    add     %r11, %r10
    mov     %r10, -8(%rbp)
    mov     -8(%rbp), %rax
    leave
    ret

.globl main

main:
    enter   $(8 * 1), $0
    mov     $10, %rdi
    mov     $5, %rsi
    mov     $0, %rax
    call    suma
    mov     %rax, -8(%rbp)
    mov     $0, %rax
    leave
    ret
```

## Ejemplo 3: While

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

## Ejemplo 4: Expresión Compleja

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
# Usar c-tds para compilar
c-tds <test>.ctds -o test.ass 
```

## Referencias

[Generación](/assembly/generation) · [Arquitectura](/assembly/architecture) · `assembly.c`
