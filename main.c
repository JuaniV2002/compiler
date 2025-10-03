#include <stdio.h>
#include "parser.tab.h"
#include "ts.h"
#include "cli.h"

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
        printf("\nEl programa es sintácticamente correcto.\n");
        return 0;
    } else {
        printf("\nEl programa no es sintácticamente correcto.\n");
        return 1;
    }
}