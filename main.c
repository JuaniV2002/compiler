#include <stdio.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"
#include "ts.h"
#include "tac.h"
#include "cli.h"
#include "semantic_analyzer.h"

extern FILE *yyin;
extern Node* root;
extern Symbol* currentMethod;

int main(int argc, char *argv[]) {
    CompilerOptions opts;
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    if (!parseArguments(argc, argv, &opts)) {
        printUsage(argv[0]);
        return 1;
    }
    
    yyin = fopen(opts.inputFile, "r");
    if (!yyin) {
        fprintf(stderr, "ERROR: no se pudo abrir el archivo '%s'\n", opts.inputFile);
        return 1;
    }
    
    if (opts.debug) {
        printDebugInfo(&opts);
    }
    
    if (yyparse() == 0) {
        
        Stack* stack = initializeTS();
        
        analyzeSemantics(root, stack);
        
        if (opts.debug) {
            printAST(root, 0);
            printTS(stack);
        }
        
        if (opts.debug) {
            TacCode* tac = initTAC();
            generateTAC(root, tac);
            printTAC(tac);
            freeTAC(tac);
        }
        
        freeAST(root);
        freeTS(stack);
        
        // No se imprime nada cuando la compilacion es exitosa sin -debug
    } else {
        freeAST(root);
        
        if (opts.debug) {
            printf("\n\033[0;31mSe encontraron errores de sintaxis.\033[0m\n\n");
        }
        
        fclose(yyin);
        return 1;
    }
    
    fclose(yyin);
    return 0;
}
