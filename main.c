#include <stdio.h>
#include "parser.tab.h"

extern FILE *yyin;

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
        printf("\n\nEl programa es sintacticamente correcto.\n\n");
    } else {
        printf("\n\nSe encontraron errores de sintaxis.\n\n");
    }

    fclose(yyin);
    return 0;
}