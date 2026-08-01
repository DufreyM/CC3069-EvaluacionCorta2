#!/bin/bash
# comparar.sh
# Compila las dos versiones y verifica que produzcan exactamente
# las mismas palabras y frecuencias para el mismo archivo de prueba.
#
# Uso: ./comparar.sh archivo.txt [numero_de_hilos]

set -e
ARCHIVO=${1:-prueba.txt}
HILOS=${2:-4}

if [ ! -f "$ARCHIVO" ]; then
    echo "ERROR: el archivo '$ARCHIVO' no existe en $(pwd)."
    echo "Verifica el nombre/ruta o coloca el archivo en esta carpeta."
    exit 1
fi

echo "Compilando..."
g++ -O2 -std=c++17 -o secuencial secuencial.cpp
g++ -O2 -std=c++17 -pthread -o paralelo paralelo.cpp

echo ""
echo "Ejecutando version secuencial sobre '$ARCHIVO'..."
./secuencial "$ARCHIVO" > /tmp/out_seq.txt
cat /tmp/out_seq.txt

echo ""
echo "Ejecutando version paralela ($HILOS hilos) sobre '$ARCHIVO'..."
./paralelo "$ARCHIVO" "$HILOS" > /tmp/out_par.txt
cat /tmp/out_par.txt

echo ""
echo "Comparando resultados (solo palabras y frecuencias)..."
grep -E ':' /tmp/out_seq.txt | grep -v "distintas\|Tiempo" > /tmp/palabras_seq.txt
grep -E ':' /tmp/out_par.txt | grep -v "distintas\|Tiempo" > /tmp/palabras_par.txt

if diff -q /tmp/palabras_seq.txt /tmp/palabras_par.txt > /dev/null; then
    echo "OK: ambas versiones produjeron exactamente las mismas palabras y frecuencias."
else
    echo "ERROR: los resultados difieren."
    diff /tmp/palabras_seq.txt /tmp/palabras_par.txt
    exit 1
fi

echo ""
echo "=== Comparacion de tiempos ==="
TIEMPO_SEQ=$(grep "Tiempo de ejecucion" /tmp/out_seq.txt | grep -oE '[0-9]+([.][0-9]+)?')
TIEMPO_PAR=$(grep "Tiempo de ejecucion" /tmp/out_par.txt | grep -oE '[0-9]+([.][0-9]+)?')

echo "Secuencial : ${TIEMPO_SEQ} ms"
echo "Paralelo   : ${TIEMPO_PAR} ms  (${HILOS} hilos)"

awk -v seq="$TIEMPO_SEQ" -v par="$TIEMPO_PAR" -v hilos="$HILOS" 'BEGIN {
    if (par <= 0) { print "No se pudo calcular el speedup (tiempo paralelo invalido)."; exit }
    speedup = seq / par
    eficiencia = (speedup / hilos) * 100
    printf "Speedup    : %.2fx\n", speedup
    printf "Eficiencia : %.1f%% (speedup / numero de hilos)\n", eficiencia
    if (speedup < 1) {
        print "crear/sincronizar hilos puede superar la diferencia; es el resultado esperado."
    }
}'
