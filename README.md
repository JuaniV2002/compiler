# Compilador

## Branches

- `main`: Versión estable del compilador.
- `parser`: Versión terminal del parser.
- `analizador-semantico`: Versión en desarrollo del analizador semantico.

## Compilación y Ejecución

Para compilar el parser:
```bash
make
```

Para ejecutar tests:
```bash
make test-correct      # Test sintácticamente correcto
make test-incorrect    # Test sintácticamente incorrecto
make test              # Ambos tests
```

Para ejecutar con archivo personalizado:
```bash
make run FILE=mi_archivo.txt
```

Para limpiar archivos generados:
```bash
make clean
```

## Salida del Programa

- Si el programa es sintácticamente correcto: "El programa es sintacticamente correcto."
- Si el programa es sintácticamente incorrecto: "El programa no es sintacticamente correcto."