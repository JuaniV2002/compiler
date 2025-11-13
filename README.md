# Compilador

## Branches

- `main`: Versión estable del compilador.
- `parser`: Versión terminal del parser.
- `analizador-semantico`: Versión terminal del analizador semantico.
- `codigo-intermedio`: Versión terminal del generador de código intermedio.
- 'assembly': Versión terminal del generador de código ensamblador.
- 'optimizations': Versión terminal con optimizaciones implementadas.

## Compilación

Para compilar el analizador semantico:
```bash
make
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

## Salida del Programa

- Si el programa es sintácticamente correcto: "El programa es sintacticamente correcto."
- Si el programa es sintácticamente incorrecto: "Se encontraron errores de sintaxis."