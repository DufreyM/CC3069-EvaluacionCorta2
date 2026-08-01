# Evaluacion Corta 2

## Archivos

| Archivo | Descripción |
|---|---|
| `secuencial.cpp` | Versión secuencial: un solo flujo de ejecución lee el archivo carácter por carácter. |
| `paralelo.cpp` | Versión paralela (fork-join) con `std::thread`: divide el archivo en N bloques de bytes, cada hilo cuenta su bloque en un diccionario local y al final se combinan (reduce) en un diccionario global. |
| `Quijote.txt` | Archivo de prueba robado de google. |
| `comparar.sh` | Compila ambas versiones, las ejecuta sobre el mismo archivo y verifica que el resultado sea idéntico. |

## Compilar

```bash
g++ -O2 -std=c++17 -o secuencial secuencial.cpp
g++ -O2 -std=c++17 -pthread -o paralelo paralelo.cpp

./secuencial archivo.txt
./paralelo archivo.txt 4      
```

## Verificación

```bash
./comparar.sh prueba.txt 4
./comparar.sh prueba_grande.txt 8
```

Se probó con 3, 4, 8 y 16 hilos sobre ambos archivos: en todos los casos la versión
paralela produce **exactamente las mismas palabras y frecuencias** que la secuencial, solo que en algunos principalmente en las pequeñas nuestro overhead hacía que la versión paralela se tardara más que la secuencial, no obstante esto mejora a partir de los 8 hilos, lo cuál es un resultado que esperabamos porque es un costo que hay que asumir. 
