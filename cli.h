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

void printUsage(char* progName);

int parseArguments(int argc, char* argv[], CompilerOptions* opts);

void printDebugInfo(CompilerOptions* opts);

#endif // CLI_H
