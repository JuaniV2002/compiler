#include <stdio.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"
#include "ts.h"

extern FILE *yyin;
extern Node* root;  // Raiz del AST

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR: Incluir el archivo a comprobar su sintaxis: %s <archivo.txt>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    if (yyparse() == 0) {
        printf("\nGenerando AST...\n");
        printAST(root, 0);

        Level* symbolTable = initializeTS();
        printTS(symbolTable);

        freeAST(root);  // Liberar memoria del AST
        freeTS(symbolTable); // Liberar memoria de la tabla de simbolos

        printf("\n\033[0;32mEl programa es sintácticamente correcto.\033[0m\n\n");
    } else {
        freeAST(root);  // Liberar memoria del AST

        printf("\n\033[0;31mSe encontraron errores de sintaxis.\033[0m\n\n");
        return 1;
    }

    fclose(yyin);
    return 0;
}