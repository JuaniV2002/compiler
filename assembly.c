#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "tac.h"
#include "assembly.h"

FuncContext* newFuncContext(char* name) {
    FuncContext* ctx = (FuncContext*) malloc(sizeof(FuncContext));
    if (!ctx) {
        fprintf(stderr, "Error al asignar memoria para contexto de funcion.\n");
        exit(EXIT_FAILURE);
    }
    ctx->funcName = name ? strdup(name) : NULL;
    ctx->stackSize = 0;
    ctx->vars = NULL;
    ctx->paramStack.count = 0;
    for (int i = 0; i < 6; i++) {
        ctx->paramStack.values[i] = NULL;
    }
    return ctx;
}

void addVar(FuncContext* ctx, char* name) {
    if (!ctx || !name) return;
    
    // Verificar si ya existe
    VarInfo* current = ctx->vars;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return;
        }
        current = current->next;
    }
    
    // Agregar nueva variable
    VarInfo* newVar = (VarInfo*) malloc(sizeof(VarInfo));
    if (!newVar) {
        fprintf(stderr, "Error al asignar memoria para variable.\n");
        exit(EXIT_FAILURE);
    }
    newVar->name = strdup(name);
    ctx->stackSize++;
    newVar->offset = ctx->stackSize * 8;
    newVar->next = ctx->vars;
    ctx->vars = newVar;
}

int getVarOffset(FuncContext* ctx, char* name) {
    if (!ctx || !name) return -1;
    
    VarInfo* current = ctx->vars;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->offset;
        }
        current = current->next;
    }
    return -1;
}

void freeFuncContext(FuncContext* ctx) {
    if (!ctx) return;
    
    if (ctx->funcName) free(ctx->funcName);
    
    VarInfo* var = ctx->vars;
    while (var) {
        VarInfo* temp = var;
        var = var->next;
        if (temp->name) free(temp->name);
        free(temp);
    }
    
    for (int i = 0; i < 6; i++) {
        if (ctx->paramStack.values[i]) {
            free(ctx->paramStack.values[i]);
        }
    }
    
    free(ctx);
}

static int isNumber(char* str) {
    if (!str || *str == '\0') return 0;
    
    int i = 0;
    if (str[0] == '-' || str[0] == '+') i++;
    
    while (str[i] != '\0') {
        if (str[i] < '0' || str[i] > '9') return 0;
        i++;
    }
    return 1;
}

static void emitLoadOperand(FILE* out, FuncContext* ctx, char* operand, char* reg) {
    if (!operand) return;
    
    if (isNumber(operand)) {
        fprintf(out, "    mov     $%s, %s\n", operand, reg);
    } else {
        int offset = getVarOffset(ctx, operand);
        if (offset >= 0) {
            fprintf(out, "    mov     -%d(%%rbp), %s\n", offset, reg);
        }
    }
}

static void emitStoreResult(FILE* out, FuncContext* ctx, char* dest, char* reg) {
    if (!dest) return;
    
    int offset = getVarOffset(ctx, dest);
    if (offset >= 0) {
        fprintf(out, "    mov     %s, -%d(%%rbp)\n", reg, offset);
    }
}

static void collectVarsAndParams(TacCode* tac, FuncContext* ctx, int* paramCount) {
    TacInstr* instr = tac->head;
    int inFunc = 0;
    *paramCount = 0;
    
    // Primera pasada: recolectar todas las variables y temporales
    while (instr) {
        if (instr->op == TAC_BEGIN_FUNC) {
            if (ctx->funcName && instr->dest && strcmp(instr->dest, ctx->funcName) == 0) {
                inFunc = 1;
            }
        } else if (instr->op == TAC_END_FUNC) {
            if (inFunc) break;
        } else if (inFunc) {
            // Agregar variables y temporales
            if (instr->dest && !isNumber(instr->dest)) {
                addVar(ctx, instr->dest);
            }
            if (instr->arg1 && !isNumber(instr->arg1) && instr->op != TAC_CALL && instr->op != TAC_LABEL && instr->op != TAC_GOTO && instr->op != TAC_IF_FALSE) {
                addVar(ctx, instr->arg1);
            }
            if (instr->arg2 && !isNumber(instr->arg2) && instr->op != TAC_IF_FALSE) {
                addVar(ctx, instr->arg2);
            }
        }
        instr = instr->next;
    }
    
    // Segunda pasada: identificar parametros (variables usadas antes de ser asignadas)
    instr = tac->head;
    inFunc = 0;
    char* assignedVars[100] = {NULL};
    int assignedCount = 0;
    
    while (instr) {
        if (instr->op == TAC_BEGIN_FUNC && ctx->funcName && instr->dest && strcmp(instr->dest, ctx->funcName) == 0) {
            inFunc = 1;
            instr = instr->next;
            continue;
        }
        if (instr->op == TAC_END_FUNC && inFunc) {
            break;
        }
        if (!inFunc) {
            instr = instr->next;
            continue;
        }
        
        // Chequear si arg1 o arg2 es usado antes de ser asignado
        for (int pass = 0; pass < 2; pass++) {
            char* var = (pass == 0) ? instr->arg1 : instr->arg2;
            if (!var || isNumber(var)) continue;
            if (instr->op == TAC_CALL || instr->op == TAC_LABEL || instr->op == TAC_GOTO) continue;
            if (pass == 1 && instr->op == TAC_IF_FALSE) continue;
            
            // Chequear si esta variable fue asignada antes
            int wasAssigned = 0;
            for (int i = 0; i < assignedCount; i++) {
                if (assignedVars[i] && strcmp(assignedVars[i], var) == 0) {
                    wasAssigned = 1;
                    break;
                }
            }
            
            // Si no está asignado y no se contó como parámetro, es un parámetro
            if (!wasAssigned) {
                int alreadyParam = 0;
                for (int i = 0; i < *paramCount; i++) {
                    VarInfo* v = ctx->vars;
                    while (v) {
                        if (strcmp(v->name, var) == 0) {
                            alreadyParam = 1;
                            break;
                        }
                        v = v->next;
                    }
                }
                if (!alreadyParam && *paramCount < 6) {
                    (*paramCount)++;
                    // Marcar como asignado para no contarlo de nuevo
                    if (assignedCount < 100) {
                        assignedVars[assignedCount++] = var;
                    }
                }
            }
        }
        
        // Marcar dest como asignado
        if (instr->dest && !isNumber(instr->dest)) {
            int found = 0;
            for (int i = 0; i < assignedCount; i++) {
                if (assignedVars[i] && strcmp(assignedVars[i], instr->dest) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && assignedCount < 100) {
                assignedVars[assignedCount++] = instr->dest;
            }
        }
        
        instr = instr->next;
    }
}

static void emitFunction(FILE* out, TacCode* tac, char* funcName) {
    FuncContext* ctx = newFuncContext(funcName);
    int paramCount = 0;
    
    // Recolectar todas las variables, temporales y parametros
    collectVarsAndParams(tac, ctx, &paramCount);
    
    // Emitir etiqueta de funcion
    if (strcmp(funcName, "main") == 0) {
        fprintf(out, ".globl main\n\n");
    }
    fprintf(out, "%s:\n", funcName);
    
    // Prologo
    fprintf(out, "    enter   $(8 * %d), $0\n", ctx->stackSize);
    
    // Guardar parametros desde registros si existen
    if (paramCount > 0) {
        char* paramRegs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        
        // Buscar el primer uso de variables antes de asignación para determinar parametros
        TacInstr* instr = tac->head;
        int inFunc = 0;
        int paramsSaved = 0;
        char* savedParams[6] = {NULL};
        
        while (instr && paramsSaved < paramCount) {
            if (instr->op == TAC_BEGIN_FUNC && strcmp(instr->dest, funcName) == 0) {
                inFunc = 1;
                instr = instr->next;
                continue;
            }
            if (!inFunc) {
                instr = instr->next;
                continue;
            }
            
            // Buscar primera variable usada que no sea numero ni llamada
            for (int pass = 0; pass < 2; pass++) {
                char* var = (pass == 0) ? instr->arg1 : instr->arg2;
                if (!var || isNumber(var)) continue;
                if (instr->op == TAC_CALL || instr->op == TAC_LABEL || instr->op == TAC_GOTO) continue;
                
                // Ver si ya guardamos este parametro
                int alreadySaved = 0;
                for (int i = 0; i < paramsSaved; i++) {
                    if (savedParams[i] && strcmp(savedParams[i], var) == 0) {
                        alreadySaved = 1;
                        break;
                    }
                }
                
                if (!alreadySaved) {
                    int offset = getVarOffset(ctx, var);
                    if (offset > 0 && paramsSaved < 6) {
                        fprintf(out, "    mov     %s, -%d(%%rbp)\n", paramRegs[paramsSaved], offset);
                        savedParams[paramsSaved] = var;
                        paramsSaved++;
                        break;
                    }
                }
            }
            
            instr = instr->next;
        }
    }
    
    // Procesar instrucciones
    TacInstr* instr = tac->head;
    int inFunc = 0;
    int hasReturn = 0;
    
    while (instr) {
        if (instr->op == TAC_BEGIN_FUNC && strcmp(instr->dest, funcName) == 0) {
            inFunc = 1;
            instr = instr->next;
            continue;
        }
        
        if (instr->op == TAC_END_FUNC && inFunc) {
            break;
        }
        
        if (!inFunc) {
            instr = instr->next;
            continue;
        }
        
        switch (instr->op) {
            case TAC_ADD:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    add     %%r11, %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_SUB:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    sub     %%r11, %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_MUL:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    imul    %%r11, %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_DIV:
                emitLoadOperand(out, ctx, instr->arg1, "%rax");
                fprintf(out, "    cqo\n");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    idiv    %%r11\n");
                emitStoreResult(out, ctx, instr->dest, "%rax");
                break;
                
            case TAC_MOD:
                emitLoadOperand(out, ctx, instr->arg1, "%rax");
                fprintf(out, "    cqo\n");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    idiv    %%r11\n");
                emitStoreResult(out, ctx, instr->dest, "%rdx");
                break;
                
            case TAC_LT:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    cmp     %%r11, %%r10\n");
                fprintf(out, "    mov     $0, %%r11\n");
                fprintf(out, "    mov     $1, %%r10\n");
                fprintf(out, "    cmovl   %%r10, %%r11\n");
                emitStoreResult(out, ctx, instr->dest, "%r11");
                break;
                
            case TAC_GT:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    cmp     %%r11, %%r10\n");
                fprintf(out, "    mov     $0, %%r11\n");
                fprintf(out, "    mov     $1, %%r10\n");
                fprintf(out, "    cmovg   %%r10, %%r11\n");
                emitStoreResult(out, ctx, instr->dest, "%r11");
                break;
                
            case TAC_EQ:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    cmp     %%r10, %%r11\n");
                fprintf(out, "    mov     $0, %%r11\n");
                fprintf(out, "    mov     $1, %%r10\n");
                fprintf(out, "    cmove   %%r10, %%r11\n");
                emitStoreResult(out, ctx, instr->dest, "%r11");
                break;
                
            case TAC_AND:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    and     %%r11, %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_OR:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitLoadOperand(out, ctx, instr->arg2, "%r11");
                fprintf(out, "    or      %%r11, %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_NOT:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                fprintf(out, "    test    %%r10, %%r10\n");
                fprintf(out, "    mov     $0, %%r10\n");
                fprintf(out, "    sete    %%r10b\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_NEG:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                fprintf(out, "    neg     %%r10\n");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_COPY:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                emitStoreResult(out, ctx, instr->dest, "%r10");
                break;
                
            case TAC_LABEL:
                fprintf(out, ".%s:\n", instr->dest);
                break;
                
            case TAC_GOTO:
                fprintf(out, "    jmp     .%s\n", instr->dest);
                break;
                
            case TAC_IF_FALSE:
                emitLoadOperand(out, ctx, instr->arg1, "%r10");
                fprintf(out, "    mov     $1, %%r11\n");
                fprintf(out, "    cmp     %%r10, %%r11\n");
                fprintf(out, "    jne     .%s\n", instr->arg2);
                break;
                
            case TAC_PARAM:
                // Guardar parametro en el stack de parametros
                if (ctx->paramStack.count < 6) {
                    ctx->paramStack.values[ctx->paramStack.count] = strdup(instr->arg1);
                    ctx->paramStack.count++;
                }
                break;
                
            case TAC_CALL: {
                // Cargar parametros en registros segun convencion
                char* regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
                
                for (int i = 0; i < ctx->paramStack.count && i < 6; i++) {
                    emitLoadOperand(out, ctx, ctx->paramStack.values[i], regs[i]);
                }
                
                // Para funciones extern variádicas, limpiar %rax
                fprintf(out, "    mov     $0, %%rax\n");
                fprintf(out, "    call    %s\n", instr->arg1);
                emitStoreResult(out, ctx, instr->dest, "%rax");
                
                // Limpiar el stack de parametros
                for (int i = 0; i < ctx->paramStack.count; i++) {
                    if (ctx->paramStack.values[i]) {
                        free(ctx->paramStack.values[i]);
                        ctx->paramStack.values[i] = NULL;
                    }
                }
                ctx->paramStack.count = 0;
                break;
            }
                
            case TAC_RETURN:
                hasReturn = 1;
                if (instr->arg1) {
                    emitLoadOperand(out, ctx, instr->arg1, "%rax");
                } else {
                    fprintf(out, "    mov     $0, %%rax\n");
                }
                fprintf(out, "    leave\n");
                fprintf(out, "    ret\n");
                break;
                
            default:
                break;
        }
        
        instr = instr->next;
    }
    
    // Epilogo (solo si no hubo return explícito)
    if (!hasReturn) {
        fprintf(out, "    mov     $0, %%rax\n");
        fprintf(out, "    leave\n");
        fprintf(out, "    ret\n");
    }
    fprintf(out, "\n");
    
    freeFuncContext(ctx);
}

void generateAssembly(TacCode* tac, FILE* output) {
    if (!tac || !output) return;
    
    // Buscar todas las funciones
    TacInstr* instr = tac->head;
    
    while (instr) {
        if (instr->op == TAC_BEGIN_FUNC && instr->dest) {
            emitFunction(output, tac, instr->dest);
        }
        instr = instr->next;
    }
}
