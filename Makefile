# Compilador y flags
CC = gcc

# Archivos fuente
BISON_SRC = parser.y
FLEX_SRC  = scanner.l
MAIN_SRC  = main.c
SYMBOL_SRC = symbol.c
AST_SRC   = ast.c
TS_SRC    = ts.c
TAC_SRC   = tac.c
CLI_SRC   = cli.c
SEMANTIC_SRC = semantic_analyzer.c
ASSEMBLY_SRC = assembly.c

# Archivos generados por bison y flex
BISON_C = parser.tab.c
BISON_H = parser.tab.h
FLEX_C  = lex.yy.c

# Ejecutable final
TARGET = c-tds

# Archivos de test
TEST_CORRECT = tests/test_correct.ctds
TEST_INCORRECT = tests/test_incorrect.ctds

# Archivo de entrada por defecto
FILE ?= $(TEST_CORRECT)

# Regla por defecto
all: $(TARGET)

# Generar ejecutable
$(TARGET): $(MAIN_SRC) $(AST_SRC) $(BISON_C) $(FLEX_C) $(TS_SRC) $(SYMBOL_SRC) $(TAC_SRC) $(CLI_SRC) $(ASSEMBLY_SRC)
	$(CC) -o $(TARGET) $(MAIN_SRC) $(AST_SRC) $(BISON_C) $(FLEX_C) $(TS_SRC) $(SYMBOL_SRC) $(TAC_SRC) $(CLI_SRC) $(SEMANTIC_SRC) $(ASSEMBLY_SRC)


# Generar parser con Bison
$(BISON_C) $(BISON_H): $(BISON_SRC)
	bison -d -Wcounterexamples $(BISON_SRC)

# Generar scanner con Flex
$(FLEX_C): $(FLEX_SRC)
	flex $(FLEX_SRC)

# Ejecutar el programa con archivo de entrada
run: $(TARGET)
	./$(TARGET) $(FILE)

# Ejecutar test correcto
test-correct: $(TARGET)
	./$(TARGET) $(TEST_CORRECT)

# Ejecutar test incorrecto
test-incorrect: $(TARGET)
	./$(TARGET) $(TEST_INCORRECT)

# Ejecutar ambos tests
test: $(TARGET)
	@echo "=== Test correcto ==="
	./$(TARGET) $(TEST_CORRECT)
	@echo "=== Test incorrecto ==="
	./$(TARGET) $(TEST_INCORRECT)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(BISON_C) $(BISON_H) $(FLEX_C)

.PHONY: all run test-correct test-incorrect test-all clean