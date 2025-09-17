// ast.h
#ifndef AST_H
#define AST_H

typedef enum {PROG, DECL, SENT, EXP, ASSIGN, RET, TERM} nodeType;

typedef enum {NONE_INFO, TYPE_VOID, TYPE_ID, TYPE_INT, TYPE_BOOL, TYPE_BIN_OP, TYPE_UN_OP} infoType;

typedef enum {T_PLUS, T_MINUS, T_MULT, T_DIVISION, T_MOD, T_LESS, T_GREATER, T_EQUAL, T_AND, T_OR} bin_op;

typedef enum {T_NOT, T_UN_MINUS} un_op;

typedef union {
    char* id;
    int int_num;
    int boolean;
    bin_op bin_op;
    un_op un_op;
} Value;

typedef struct Node {
    nodeType t_Node;
    infoType t_Info;

    Value* info;

    struct Node* left;
    struct Node* right;
} Node;

Node* newNode_Terminal(infoType type, Value value);

Node* newNode_NonTerminal(nodeType type, infoType infType, Value value, struct Node* left, struct Node* right);

void printAST(struct Node* root, int level);

void freeAST(struct Node* root);

#endif // AST_H