#include <map>
#include <vector>
#include <thread>
#include <string>
#include <cctype>
#include <chrono>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <pthread.h>
#endif

using Frecuencia = std::map<std::string, long long>;

inline bool esCaracterDePalabra(unsigned char c) {
    return std::isalnum(c) || c >= 0x80;
}

bool fijarAfinidadNucleo(std::thread& hilo, unsigned int nucleo) {
#if defined(_WIN32)
    HANDLE h = static_cast<HANDLE>(hilo.native_handle());
    DWORD_PTR mascara = (DWORD_PTR)1 << nucleo;
    return SetThreadAffinityMask(h, mascara) != 0;
#elif defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(nucleo, &cpuset);
    int resultado = pthread_setaffinity_np(hilo.native_handle(), sizeof(cpu_set_t), &cpuset);
    return resultado == 0;
#else
    (void)hilo; (void)nucleo;
    return false;
#endif
}

void procesarBloque(const std::string& rutaArchivo,
                     std::streampos inicio,
                     std::streampos fin,
                     bool esPrimerBloque,
                     bool esUltimoBloque,
                     Frecuencia& frecuenciaLocal) {
    std::ifstream archivo(rutaArchivo, std::ios::binary);
    if (!archivo.is_open()) return;

    if (!esPrimerBloque) {
        archivo.seekg(inicio);
        char anterior;
        archivo.seekg(-1, std::ios::cur);
        archivo.get(anterior);
        if (esCaracterDePalabra(static_cast<unsigned char>(anterior))) {
            char c;
            while (archivo.get(c)) {
                if (!esCaracterDePalabra(static_cast<unsigned char>(c))) break;
            }
        } else {
            archivo.seekg(inicio); 
        }
    } else {
        archivo.seekg(inicio);
    }

    std::string palabra = "";
    char c;
    std::streampos posActual = archivo.tellg();

    while (archivo.get(c)) {
        posActual = archivo.tellg();

        if (esCaracterDePalabra(static_cast<unsigned char>(c))) {
            palabra += c;
        } else {
            if (!palabra.empty()) {
                auto it = frecuenciaLocal.find(palabra);
                if (it != frecuenciaLocal.end()) it->second += 1;
                else frecuenciaLocal[palabra] = 1;
                palabra = "";
            }
            if (!esUltimoBloque && posActual >= fin) break;
        }
    }

    // Si el archivo termino con una palabra pendiente
    if (!palabra.empty()) {
        auto it = frecuenciaLocal.find(palabra);
        if (it != frecuenciaLocal.end()) it->second += 1;
        else frecuenciaLocal[palabra] = 1;
    }

    archivo.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <archivo.txt> [numero_de_hilos]\n";
        return 1;
    }

    std::string rutaArchivo = argv[1];
    unsigned int numHilos = (argc >= 3) ? std::stoul(argv[2])
                                         : std::thread::hardware_concurrency();
    if (numHilos == 0) numHilos = 4;

    std::ifstream archivoCheck(rutaArchivo, std::ios::binary | std::ios::ate);
    if (!archivoCheck.is_open()) {
        std::cerr << "ERROR: no se pudo abrir el archivo: " << rutaArchivo << "\n";
        return 1;
    }
    std::streampos tamanoArchivo = archivoCheck.tellg();
    archivoCheck.close();

    if (tamanoArchivo == 0) {
        std::cout << "=== Resultado (PARALELO) ===\n";
        std::cout << "(archivo vacio)\n";
        return 0;
    }

    if ((std::streamoff)numHilos > (std::streamoff)tamanoArchivo)
        numHilos = static_cast<unsigned int>(tamanoArchivo);

    auto inicioReloj = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> hilos;
    std::vector<Frecuencia> frecuenciasLocales(numHilos);

    std::streamoff tam = tamanoArchivo;
    std::streamoff bloque = tam / numHilos;

    unsigned int nucleosDisponibles = std::thread::hardware_concurrency();
    if (nucleosDisponibles == 0) nucleosDisponibles = 1;

    for (unsigned int i = 0; i < numHilos; ++i) {
        std::streampos inicio = i * bloque;
        std::streampos fin = (i == numHilos - 1) ? tam : (i + 1) * bloque;
        bool esPrimerBloque = (i == 0);
        bool esUltimoBloque = (i == numHilos - 1);

        hilos.emplace_back(procesarBloque, rutaArchivo, inicio, fin,
                            esPrimerBloque, esUltimoBloque,
                            std::ref(frecuenciasLocales[i]));

        // asignacion de este hilo a un nucleo fisico
        // Si hay mas hilos que nucleos, se reparten en round-robin
        unsigned int nucleoAsignado = i % nucleosDisponibles;
        bool ok = fijarAfinidadNucleo(hilos[i], nucleoAsignado);
        std::cout << "Hilo " << i << " -> nucleo fisico " << nucleoAsignado
                   << (ok ? "" : " (no soportado en esta plataforma, el SO decide)")
                   << "\n";
    }

    for (auto& h : hilos) h.join();

    // sumar los diccionarios locales en uno global ---
    Frecuencia frecuenciaGlobal;
    for (const auto& local : frecuenciasLocales) {
        for (const auto& par : local) {
            frecuenciaGlobal[par.first] += par.second;
        }
    }

    auto finReloj = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(finReloj - inicioReloj).count();

    std::cout << "Resultado (PARALELO, " << numHilos << " hilos) ===\n";
    for (const auto& par : frecuenciaGlobal) {
        std::cout << par.first << ": " << par.second << "\n";
    }
    std::cout << "Palabras distintas: " << frecuenciaGlobal.size() << "\n";
    std::cout << "Tiempo de ejecucion: " << ms << " ms\n";

    return 0;
}
