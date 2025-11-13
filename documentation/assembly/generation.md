---
title: "Generación de Código Assembly"
description: "Algoritmos de traducción de TAC a x86-64."
---

# Generación de Código Assembly

## Algoritmo Principal

```c
void generateAssembly(TacCode* tac, FILE* output) {
    // Buscar todas las funciones (TAC_BEGIN_FUNC)
    // Para cada función, llamar emitFunction()
}
```

Recorre el TAC y genera código para cada función.

## Gestión del Stack Frame

**Asignación de offsets**:
```c
void addVar(FuncContext* ctx, char* name) {
    ctx->stackSize++;
    int offset = ctx->stackSize * 8;  // 8 bytes por qword
    // Agregar VarInfo a la lista
}

int getVarOffset(FuncContext* ctx, char* name) {
    // Buscar en la lista y retornar offset
}
```

**Resultado**:
```
-8(%rbp)   → Variable 1
-16(%rbp)  → Variable 2
```

## Recolección de Variables

`collectVarsAndParams()` hace dos pasadas:

1. **Primera pasada**: Agrega todas las variables, temporales y argumentos al contexto
2. **Segunda pasada**: Identifica parámetros (variables usadas antes de ser asignadas)

## Generación de Función

`emitFunction()` genera código completo:

```c
static void emitFunction(FILE* out, TacCode* tac, char* funcName) {
    FuncContext* ctx = newFuncContext(funcName);
    collectVarsAndParams(tac, ctx, &paramCount);
    
    // Etiqueta y prólogo
    fprintf(out, "%s:\n", funcName);
    fprintf(out, "    enter $(8 * %d), $0\n", ctx->stackSize);
    
    // Guardar parámetros desde registros
    // Procesar instrucciones TAC
    // Epílogo (si no hay return explícito)
    
    freeFuncContext(ctx);
}
```

## Traducción de Operaciones

**Operaciones binarias** (TAC: `t0 = a + b`):
```c
emitLoadOperand(out, ctx, arg1, "%r10");
emitLoadOperand(out, ctx, arg2, "%r11");
fprintf(out, "    add %%r11, %%r10\n");
emitStoreResult(out, ctx, dest, "%r10");
```

**División** (TAC: `t0 = x / y`):
```c
emitLoadOperand(out, ctx, arg1, "%rax");
fprintf(out, "    cqo\n");
emitLoadOperand(out, ctx, arg2, "%r11");
fprintf(out, "    idiv %%r11\n");
emitStoreResult(out, ctx, dest, "%rax");  // Cociente
```

**Comparación ==** (TAC: `t0 = x == y`):
```c
emitLoadOperand(out, ctx, arg1, "%r10");
emitLoadOperand(out, ctx, arg2, "%r11");
fprintf(out, "    cmp %%r10, %%r11\n");   // Compara r11 con r10 (!)
fprintf(out, "    mov $0, %%r11\n");
fprintf(out, "    mov $1, %%r10\n");
fprintf(out, "    cmove %%r10, %%r11\n");  // if r11 == r10: r11 = 1
emitStoreResult(out, ctx, dest, "%r11");
```

:::callout warning Inconsistencia
Nota que `TAC_EQ` invierte el orden en `cmp` comparado con `TAC_LT` y `TAC_GT`. Esto funciona porque la igualdad es simétrica, pero puede ser confuso.
:::

**Operaciones lógicas** (TAC: `t0 = a && b`):
```c
emitLoadOperand(out, ctx, arg1, "%r10");
emitLoadOperand(out, ctx, arg2, "%r11");
fprintf(out, "    and %%r11, %%r10\n");
emitStoreResult(out, ctx, dest, "%r10");
```

## Control de Flujo

**Etiquetas** (TAC: `L0:`):
```c
fprintf(out, ".%s:\n", dest);
```

**Goto** (TAC: `goto L0`):
```c
fprintf(out, "    jmp .%s\n", dest);
```

**If** (TAC: `if !t0 goto L0`):
```c
emitLoadOperand(out, ctx, arg1, "%r10");
fprintf(out, "    mov $1, %%r11\n");
fprintf(out, "    cmp %%r10, %%r11\n");
fprintf(out, "    jne .%s\n", arg2);
```

## Llamadas a Funciones

**Parámetros** (TAC: `param x`):
```c
// Acumular en paramStack
ctx->paramStack.values[ctx->paramStack.count++] = arg1;
```

**Llamada** (TAC: `t0 = call suma`):
```c
char* paramRegs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
for (int i = 0; i < ctx->paramStack.count; i++) {
    emitLoadOperand(out, ctx, ctx->paramStack.values[i], paramRegs[i]);
}
fprintf(out, "    mov $0, %%rax\n");  // Limpiar rax
fprintf(out, "    call %s\n", arg1);
emitStoreResult(out, ctx, dest, "%rax");
ctx->paramStack.count = 0;
```

## Funciones Auxiliares

`emitLoadOperand()`: Carga operando a registro
```c
if (isNumber(operand)) {
    fprintf(out, "    mov $%s, %s\n", operand, reg);
} else {
    int offset = getVarOffset(ctx, operand);
    fprintf(out, "    mov -%d(%%rbp), %s\n", offset, reg);
}
```

`emitStoreResult()`: Guarda registro en variable
```c
int offset = getVarOffset(ctx, dest);
fprintf(out, "    mov %s, -%d(%%rbp)\n", reg, offset);
```

`isNumber()`: Verifica si string es número

## Referencias

[Arquitectura](assembly/architecture) · [Ejemplos](assembly/examples) · `assembly.c`
