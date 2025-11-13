# Compilador C-TDS

## Branches

- `main`: Versión estable del compilador.
- `parser`: Versión terminal del parser.
- `analizador-semantico`: Versión terminal del analizador semantico.
- `codigo-intermedio`: Versión terminal del generador de código intermedio.
- `assembly`: Versión terminal del generador de código ensamblador.
- `optimizations`: Versión terminal con optimizaciones implementadas.

## Compilación

Para compilar el compilador:
```bash
make
```

Esto genera el ejecutable `c-tds`.

## Uso

### Sintaxis
```bash
c-tds [opcion] nombreArchivo.ctds
```

### Opciones

- **`-o <salida>`**: Renombra el archivo ejecutable a `<salida>`
- **`-target <etapa>`**: Compila hasta la etapa especificada
  - `scan`: Análisis léxico (genera `.lex`)
  - `parse`: Análisis sintáctico (genera `.sint`)
  - `codinter`: Código intermedio (genera `.ci`)
  - `assembly`: Ensamblador (genera `.ass`)
- **`-opt [optimización]`**: Realiza optimizaciones. `all` realiza todas las optimizaciones soportadas
- **`-debug`**: Imprime información de debugging. Si la opción no es dada, cuando la compilación es exitosa no se imprime ninguna salida

### Ejemplos

Compilar hasta análisis sintáctico (comportamiento por defecto):
```bash
./c-tds programa.ctds
```

Compilar con información de debugging:
```bash
./c-tds -debug programa.ctds
```

Compilar hasta la etapa de análisis léxico:
```bash
./c-tds -target scan programa.ctds
```

Especificar nombre de salida:
```bash
./c-tds -o mi_programa programa.ctds
```

Combinar múltiples opciones:
```bash
./c-tds -target assembly -o programa.asm -opt all -debug programa.ctds
```

## Tests

Para ejecutar tests usando el Makefile:
```bash
make test-correct      # Test sintácticamente correcto
make test-incorrect    # Test sintácticamente incorrecto
make test              # Ambos tests
```

Para ejecutar con archivo personalizado:
```bash
make run FILE=mi_archivo.txt
```

## Limpieza

Para limpiar archivos generados:
```bash
make clean
```

## Notas

- El archivo de entrada debe tener extensión `.ctds`.
- El nombre del archivo no puede comenzar con `-`.
- Por defecto, sin especificar `-target`, la compilación procede hasta la etapa corriente.
- Sin `-o`, el archivo de salida tiene el nombre base del archivo de entrada con extensión según la etapa (`.out` por defecto).
- Sin `-debug`, cuando la compilación es exitosa no se imprime el AST ni la tabla de símbolos.