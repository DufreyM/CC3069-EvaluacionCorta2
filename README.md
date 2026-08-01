# Evaluacion Corta 2
Video : https://canva.link/m6yv1qglxm3n031
## Archivos

| Archivo | Descripción |
|---|---|
| `diagrama_flujo_paralelo.png` | Diagrama de flujo de la versión paralela: lectura del archivo, división en bloques, creación de hilos, conteo parcial por hilo, join (sincronización) y combinación de resultados. |
| `secuencial.cpp` | Versión secuencial: un solo flujo de ejecución lee el archivo carácter por carácter. |
| `paralelo.cpp` | Versión paralela (fork-join) con `std::thread`: divide el archivo en N bloques de bytes, cada hilo cuenta su bloque en un diccionario local y al final se combinan (reduce) en un diccionario global. |
| `Quijote.txt` | Archivo de prueba (texto completo de El Quijote, UTF-8). |
| `comparar.sh` | Compila ambas versiones, las ejecuta sobre el mismo archivo y verifica que el resultado sea idéntico. |

## Nota sobre codificación

El tokenizador considera parte de una palabra cualquier byte >= 0x80, además de
`isalnum`. Esto es necesario porque el archivo está en UTF-8 y las vocales con
tilde y la ñ se codifican con bytes fuera del rango ASCII: sin este ajuste,
`std::isalnum` corta la palabra a la mitad (p. ej. "después" se leería como
"despu" + "s"). La misma función (`esCaracterDePalabra`) se usa en ambas
versiones para garantizar que el tokenizado sea idéntico.

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
