---
title: "Gramática C-TDS"
description: "Reglas de producción y precedencia de operadores."
---

# Gramática C-TDS

## Estructura del Programa

La regla principal es:

```yacc
PROG : T_PROG T_OPENB T_CLOSEB
     | T_PROG T_OPENB VAR_DECLS T_CLOSEB
     | T_PROG T_OPENB METHOD_DECLS T_CLOSEB
     | T_PROG T_OPENB VAR_DECLS METHOD_DECLS T_CLOSEB
```

Básicamente: `program { [variables] [métodos] }`

## Declaraciones de Variables

```yacc
VAR_DECL : Type T_ID T_ASSIGN EXPR T_SEMIC

Type : T_INTEGER | T_BOOL
```

Ejemplo: `integer x = 5;`

## Declaraciones de Métodos

```yacc
METHOD_PROFILE : Type T_ID T_OPENP
               | T_VOID T_ID T_OPENP

METHOD_DECL : METHOD_PROFILE PARAMS T_CLOSEP BLOCK_OR_EXTERN

BLOCK_OR_EXTERN : BLOCK
                | T_EXTERN T_SEMIC
```

Los métodos pueden ser `extern` (solo declaración) o tener un cuerpo con código.

**Ejemplo**:
```c
integer suma(integer a, integer b) { ... }
```

## Sentencias

```yacc
STATEMENT : T_ID T_ASSIGN EXPR T_SEMIC              // Asignación
          | METHOD_CALL T_SEMIC                      // Llamada a método
          | T_IF T_OPENP EXPR T_CLOSEP T_THEN BLOCK ELSE_ST    // If/else
          | T_WHILE EXPR BLOCK                       // While
          | T_RETURN EXPR_ST T_SEMIC                 // Return
          | BLOCK                                    // Bloque anidado
```

## Expresiones

### Precedencia de Operadores

En `parser.y` las declaramos de menor a mayor precedencia:

```yacc
%left   T_OR                      // ||
%left   T_AND                     // &&
%left   T_EQUAL                   // ==
%left   T_LESS T_GREATER          // < >
%left   T_PLUS T_MINUS            // + -
%left   T_MULT T_DIVISION T_MOD   // * / %
%right  T_NOT                     // !
%right  U_MINUS                   // - (unario)
```

:::callout tip Precedencia
Los de arriba se evalúan después. Por ejemplo, `*` tiene más prioridad que `+`, entonces `3 + 5 * 2` se interpreta como `3 + (5 * 2)`.
:::

### Operaciones Binarias

```yacc
EXPR : EXPR T_PLUS EXPR        // +
     | EXPR T_MINUS EXPR       // -
     | EXPR T_MULT EXPR        // *
     | EXPR T_DIVISION EXPR    // /
     | EXPR T_MOD EXPR         // %
     | EXPR T_LESS EXPR        // <
     | EXPR T_GREATER EXPR     // >
     | EXPR T_EQUAL EXPR       // ==
     | EXPR T_AND EXPR         // &&
     | EXPR T_OR EXPR          // ||
```

### Operaciones Unarias

```yacc
EXPR : T_MINUS EXPR %prec U_MINUS    // Negación: -x
     | T_NOT EXPR                     // NOT lógico: !x
```

La directiva `%prec U_MINUS` indica que el `-` unario tiene alta precedencia (no es lo mismo que el binario).

### Literales y Variables

```yacc
LITERAL : INT_NUM
        | T_TRUE
        | T_FALSE

EXPR : T_ID                           // Variable
     | METHOD_CALL                    // Llamada a método
     | LITERAL                        // Literal
     | T_OPENP EXPR T_CLOSEP          // Paréntesis
```

### Llamadas a Métodos

```yacc
METHOD_CALL : T_ID T_OPENP EXPRS T_CLOSEP

EXPRS : /* vacío */
      | EXPRS T_COMMA EXPR
      | EXPR
```

## Acciones Semánticas

Cada regla construye un nodo del AST usando `newNode_NonTerminal()` o `newNode_Terminal()`:

```yacc
EXPR : EXPR T_PLUS EXPR {
    $$ = newNode_NonTerminal(N_PLUS, NULL, $1, $3, NULL);
}
```

El `$$` es el valor de retorno, y `$1`, `$3` son los valores de los componentes de la regla.
