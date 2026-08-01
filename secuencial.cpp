// Version SECUENCIAL del algoritmo de conteo de frecuencia de palabras.
// Compilar:  g++ -O2 -std=c++17 -o secuencial secuencial.cpp
// Ejecutar:  ./secuencial archivo.txt

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <chrono>


using Frecuencia = std::map<std::string, long long>;

// Procesa un archivo completo caracter por caracter 
Frecuencia contarPalabras(const std::string& rutaArchivo) {
    Frecuencia frecuencia; 
    // Inicializar diccionario frecuencia = {} como lo definimos en el diagrama

    std::ifstream archivo(rutaArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        std::cerr << "ERROR: no se pudo abrir el archivo: " << rutaArchivo << "\n";
        return frecuencia; 
    }

    std::string palabra = "";
    char c;

    // Mientras existan caracteres por leer en el documento
    while (archivo.get(c)) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            // c es letra o numero -> palabra = palabra + c
            palabra += c;
        } else {
            // Es espacio o caracter especial: cerrar la palabra actual
            if (!palabra.empty()) {
                // ¿palabra existe en frecuencia?
                auto it = frecuencia.find(palabra);
                if (it != frecuencia.end()) {
                    it->second += 1; // incrementar frecuencia
                } else {
                    frecuencia[palabra] = 1; // registrar con frecuencia 1
                }
                palabra = ""; // palabra = ""
            }
        }
    }

    // Fin del archivo: si quedo una palabra pendiente sin cerrar, procesarla
    if (!palabra.empty()) {
        auto it = frecuencia.find(palabra);
        if (it != frecuencia.end()) {
            it->second += 1;
        } else {
            frecuencia[palabra] = 1;
        }
    }

    archivo.close();
    return frecuencia;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.txt>\n";
        return 1;
    }

    std::string rutaArchivo = argv[1];

    auto inicio = std::chrono::high_resolution_clock::now();
    Frecuencia frecuencia = contarPalabras(rutaArchivo);
    auto fin = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(fin - inicio).count();

    // Mostrar las frecuencias almacenadas
    std::cout << "Resultado\n";
    for (const auto& par : frecuencia) {
        std::cout << par.first << ": " << par.second << "\n";
    }
    std::cout << "Palabras distintas: " << frecuencia.size() << "\n";
    std::cout << "Tiempo de ejecucion: " << ms << " ms\n";

    return 0;
}
