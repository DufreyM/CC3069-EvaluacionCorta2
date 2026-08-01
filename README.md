# Práctica Guiada 01 – Parte 02: Conteo de frecuencia de palabras (secuencial vs. paralelo)

## Archivos

| Archivo | Descripción |
|---|---|
| `secuencial.cpp` | Versión secuencial: un solo flujo de ejecución lee el archivo carácter por carácter. |
| `paralelo.cpp` | Versión paralela (fork-join) con `std::thread`: divide el archivo en N bloques de bytes, cada hilo cuenta su bloque en un diccionario local y al final se combinan (reduce) en un diccionario global. |
| `prueba.txt` | Archivo de prueba pequeño (frases con palabras repetidas). |
| `prueba_grande.txt` | Archivo de prueba generado (≈187 KB, 20 000 tokens) para probar con más hilos y validar bordes de bloque. |
| `comparar.sh` | Compila ambas versiones, las ejecuta sobre el mismo archivo y verifica que el resultado sea idéntico. |

## Compilar y ejecutar manualmente

```bash
g++ -O2 -std=c++17 -o secuencial secuencial.cpp
g++ -O2 -std=c++17 -pthread -o paralelo paralelo.cpp

./secuencial archivo.txt
./paralelo archivo.txt 4      
```

## Verificación automática

```bash
./comparar.sh prueba.txt 4
./comparar.sh prueba_grande.txt 8
```

Se probó con 3, 4, 8 y 16 hilos sobre ambos archivos: en todos los casos la versión
paralela produce **exactamente las mismas palabras y frecuencias** que la secuencial.
