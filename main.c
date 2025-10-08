#include <stdio.h>
#include "parser.tab.h"

extern FILE *yyin;

int main(int argc, char *argv[]) {
    int ok = 0;

    if (argc >= 2) {
        yyin = fopen(argv[1], "r");
        if (yyin) {
            ok = (yyparse() == 0);  // usa [`yyparse`](parser.tab.c)
            fclose(yyin);
        }
    }

    if (ok) {
        printf("\n\033[0;32mEl programa es sintácticamente correcto.\033[0m\n\n");
        return 0;
    } else {
        printf("\n\033[0;31mSe encontraron errores de sintaxis.\033[0m\n\n");
        return 1;
    }
}