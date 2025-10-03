#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

void printUsage(char* progName) {
    fprintf(stderr, "Uso: %s [opcion] nombreArchivo.ctds\n", progName);
    fprintf(stderr, "\nOpciones:\n");
    fprintf(stderr, "  -o <salida>          Renombra el archivo ejecutable a <salida>\n");
    fprintf(stderr, "  -target <etapa>      Compila hasta <etapa> (scan, parse, codinter, assembly)\n");
    fprintf(stderr, "  -opt [optimizacion]  Realiza optimizaciones (all para todas)\n");
    fprintf(stderr, "  -debug               Imprime informacion de debugging\n");
}

int validateFilename(char* filename) {
    if (!filename || filename[0] == '-') {
        return 0;
    }
    
    int len = strlen(filename);
    if (len < 6) {
        return 0;
    }
    
    return strcmp(filename + len - 5, ".ctds") == 0;
}

char* getBaseFilename(char* filename) {
    char* base = strdup(filename);
    char* dot = strrchr(base, '.');
    if (dot) {
        *dot = '\0';
    }
    return base;
}

char* getOutputExtension(TargetStage target) {
    switch (target) {
        case TARGET_SCAN:      return ".lex";
        case TARGET_PARSE:     return ".sint";
        case TARGET_CODINTER:  return ".ci";
        case TARGET_ASSEMBLY:  return ".ass";
        default:               return ".out";
    }
}

int parseArguments(int argc, char* argv[], CompilerOptions* opts) {
    opts->inputFile = NULL;
    opts->outputFile = NULL;
    opts->target = TARGET_NONE;
    opts->optimization = NULL;
    opts->debug = 0;
    
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "ERROR: -o requiere un nombre de archivo\n");
                return 0;
            }
            opts->outputFile = argv[++i];
        }
        else if (strcmp(argv[i], "-target") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "ERROR: -target requiere una etapa\n");
                return 0;
            }
            i++;
            if (strcmp(argv[i], "scan") == 0) {
                opts->target = TARGET_SCAN;
            } else if (strcmp(argv[i], "parse") == 0) {
                opts->target = TARGET_PARSE;
            } else if (strcmp(argv[i], "codinter") == 0) {
                opts->target = TARGET_CODINTER;
            } else if (strcmp(argv[i], "assembly") == 0) {
                opts->target = TARGET_ASSEMBLY;
            } else {
                fprintf(stderr, "ERROR: etapa invalida '%s'\n", argv[i]);
                return 0;
            }
        }
        else if (strcmp(argv[i], "-opt") == 0) {
            if (i + 1 < argc && argv[i + 1][0] != '-' && !validateFilename(argv[i + 1])) {
                opts->optimization = argv[++i];
            } else {
                opts->optimization = "all";
            }
        }
        else if (strcmp(argv[i], "-debug") == 0) {
            opts->debug = 1;
        }
        else if (argv[i][0] != '-') {
            if (opts->inputFile) {
                fprintf(stderr, "ERROR: multiples archivos de entrada especificados\n");
                return 0;
            }
            opts->inputFile = argv[i];
        }
        else {
            fprintf(stderr, "ERROR: opcion desconocida '%s'\n", argv[i]);
            return 0;
        }
        i++;
    }
    
    if (!opts->inputFile) {
        fprintf(stderr, "ERROR: no se especifico archivo de entrada\n");
        return 0;
    }
    
    if (!validateFilename(opts->inputFile)) {
        fprintf(stderr, "ERROR: el archivo debe tener extension .ctds y no puede empezar con '-'\n");
        return 0;
    }
    
    if (!opts->outputFile) {
        char* base = getBaseFilename(opts->inputFile);
        char* ext = getOutputExtension(opts->target);
        opts->outputFile = malloc(strlen(base) + strlen(ext) + 1);
        sprintf(opts->outputFile, "%s%s", base, ext);
        free(base);
    }
    
    return 1;
}

void printDebugInfo(CompilerOptions* opts) {
    printf("Compilando '%s'...\n", opts->inputFile);
    printf("Salida: %s\n", opts->outputFile);
    if (opts->target != TARGET_NONE) {
        printf("Target: ");
        switch (opts->target) {
            case TARGET_SCAN:      printf("scan\n"); break;
            case TARGET_PARSE:     printf("parse\n"); break;
            case TARGET_CODINTER:  printf("codinter\n"); break;
            case TARGET_ASSEMBLY:  printf("assembly\n"); break;
            default: break;
        }
    }
    if (opts->optimization) {
        printf("Optimizacion: %s\n", opts->optimization);
    }
}
