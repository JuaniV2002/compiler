#include <stdio.h>
#include "ast.h"
#include "parser.tab.h"

extern FILE *yyin;
extern Node* root;  // Raiz del AST

int main(int argc, char *argv[]) {
    int ok = 0;

    if (argc >= 2) {
        yyin = fopen(argv[1], "r");
        if (yyin) {
            ok = (yyparse() == 0);  // usa [`yyparse`](parser.tab.c)
            fclose(yyin);
            
            if (ok && root) {
                printf("\nGenerando AST...\n");
                printAST(root, 0);
                freeAST(root);  // Liberar memoria del AST
            }
        }
    }

    if (ok) {
        printf("\nEl programa es sintácticamente correcto.\n");
        return 0;
    } else {
        printf("\nEl programa no es sintácticamente correcto.\n");
        return 1;
    }
}