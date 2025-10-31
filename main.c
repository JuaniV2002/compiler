#include <stdio.h>
#include "symbol.h"
#include "ast.h"
#include "parser.tab.h"
#include "ts.h"
#include "tac.h"
#include "cli.h"
#include "semantic_analyzer.h"
#include "assembly.h"

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
        
        Level* symbolTable = initializeTS();
        
        analyzeSemantics(root, symbolTable);
        
        if (opts.debug) {
            printAST(root, 0);
            printTS(symbolTable);
        }
        
        TacCode* tac = initTAC();
        generateTAC(root, tac);
        
        if (opts.debug) {
            printTAC(tac);
        }
        
        // Manejar diferentes targets
        if (opts.target == TARGET_CODINTER) {
            // Escribir TAC a archivo .ci
            if (opts.outputFile) {
                FILE* ciFile = fopen(opts.outputFile, "w");
                if (ciFile) {
                    writeTAC(tac, ciFile);
                    fclose(ciFile);
                } else {
                    fprintf(stderr, "ERROR: no se pudo crear el archivo de salida '%s'\n", opts.outputFile);
                }
            }
        } else {
            // Generar assembly (target assembly o por defecto)
            if (opts.outputFile) {
                FILE* asmFile = fopen(opts.outputFile, "w");
                if (asmFile) {
                    generateAssembly(tac, asmFile);
                    fclose(asmFile);
                } else {
                    fprintf(stderr, "ERROR: no se pudo crear el archivo de salida '%s'\n", opts.outputFile);
                }
            } else {
                generateAssembly(tac, stdout);
            }
        }
        
        freeTAC(tac);
        freeAST(root);
        freeTS(symbolTable);
        
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
