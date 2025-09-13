# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra

# Archivos fuente
BISON_SRC = parser.y
FLEX_SRC  = scanner.l
MAIN_SRC  = main.c

# Archivos generados por bison y flex
BISON_C = parser.tab.c
BISON_H = parser.tab.h
FLEX_C  = lex.yy.c

# Ejecutable final
TARGET = parser

# Archivo de entrada por defecto
FILE ?= test.txt

# Regla por defecto
all: $(TARGET)

# Generar ejecutable
$(TARGET): $(MAIN_SRC) $(BISON_C) $(FLEX_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN_SRC) $(BISON_C) $(FLEX_C)

# Generar parser con Bison
$(BISON_C) $(BISON_H): $(BISON_SRC)
	bison -d -Wcounterexamples $(BISON_SRC)

# Generar scanner con Flex
$(FLEX_C): $(FLEX_SRC)
	flex -d $(FLEX_SRC)

# Ejecutar el programa con archivo de entrada
run: $(TARGET)
	./$(TARGET) $(FILE)

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(BISON_C) $(BISON_H) $(FLEX_C)
