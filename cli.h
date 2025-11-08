// cli.h
#ifndef CLI_H
#define CLI_H

typedef enum { TARGET_SCAN, TARGET_PARSE, TARGET_CODINTER, TARGET_ASSEMBLY, TARGET_NONE } TargetStage;

typedef struct {
    char* inputFile;
    char* outputFile;
    TargetStage target;
    char* optimization;
    int debug;
} CompilerOptions;

// Imprime el mensaje de uso del compilador
void printUsage(char* progName);

// Procesa los argumentos de linea de comandos y retorna 1 si son validos
int parseArguments(int argc, char* argv[], CompilerOptions* opts);

// Imprime informacion de depuracion sobre las opciones configuradas
void printDebugInfo(CompilerOptions* opts);

#endif // CLI_H
