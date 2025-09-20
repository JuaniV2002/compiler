// ast.h
#ifndef AST_H
#define AST_H

typedef enum {
    PROG, DECL, SENT, EXP, ASSIGN, RET, TERM,
    METHOD, PARAM, PARAMS, BLOCK, EXPRS, 
    LITERAL, METHOD_CALL, EXTERN, WHILE, IF
} nodeType;

typedef enum {
    NONE_INFO, TYPE_VOID, TYPE_ID, TYPE_INTEGER, TYPE_BOOL, 
    TYPE_BIN_OP, TYPE_UN_OP, VOID
} infoType;

typedef enum {
    T_UN_MINUS, T_UN_NOT
} un_op;

typedef union {
    char* id;
    int int_num;
    int boolean;
    int bin_op;
    un_op un_op;
} Value;

typedef struct Node {
    nodeType t_Node;
    infoType t_Info;

    Value* info;

    struct Node* left;
    struct Node* right;
    struct Node* third;  // Para nodos que necesitan un tercer hijo
    struct Node* fourth; // Para nodos que necesitan un cuarto hijo
} Node;

Node* newNode_Terminal(infoType type, Value value);

Node* newNode_NonTerminal(nodeType type, infoType infType, Value value, 
                         struct Node* left, struct Node* right);
Node* newNode_NonTerminal3(nodeType type, infoType infType, Value value,
                          struct Node* left, struct Node* right, struct Node* third);
Node* newNode_NonTerminal4(nodeType type, infoType infType, Value value,
                          struct Node* left, struct Node* right, 
                          struct Node* third, struct Node* fourth);

void printAST(struct Node* root, int level);

void freeAST(struct Node* root);

#endif // AST_H