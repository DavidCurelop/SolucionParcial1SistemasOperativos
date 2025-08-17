
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstring> // strcmp
#include <memory>

#include "persona.h"
#include "generador.h"
#include "analytics.h"
#include "utils.h"
#include "persona_c.h"

using clk = std::chrono::high_resolution_clock;

static PersonaC aStruct(const Persona& p) {
    PersonaC s{};
    std::snprintf(s.nombre,  sizeof(s.nombre),  "%s", p.getNombre().c_str());
    std::snprintf(s.apellido,sizeof(s.apellido),"%s", p.getApellido().c_str());
    std::snprintf(s.id,      sizeof(s.id),      "%s", p.getId().c_str());
    std::snprintf(s.ciudad,  sizeof(s.ciudad),  "%s", p.getCiudadNacimiento().c_str());
    Fecha f = parseFecha(p.getFechaNacimiento());
    s.dia = f.dia; s.mes = f.mes; s.anio = f.anio;
    s.ingresos = p.getIngresosAnuales();
    s.patrimonio = p.getPatrimonio();
    s.deudas = p.getDeudas();
    s.declarante = p.getDeclaranteRenta();
    return s;
}

static void uso() {
    std::cout << "Uso: ./parcial N [modo] [--csv=salida.csv]\n";
    std::cout << "  N: numero de personas a generar\n";
    std::cout << "  modo:\n";
    std::cout << "    clase_valores  (default)\n";
    std::cout << "    clase_ptrs\n";
    std::cout << "    struct_valores\n";
    std::cout << "    struct_ptrs\n";
}

static void marca(const std::string& etiqueta,
                  const clk::time_point& t0,
                  size_t& picoMB) {
    auto now = clk::now();
    double secs = std::chrono::duration<double>(now - t0).count();
    size_t mem = memoriaRSS_MB();
    if (mem > picoMB) picoMB = mem;
    std::cout << "[Metrica] " << etiqueta << " | t=" << secs << " s"
              << " | RSS=" << mem << " MB"
              << " | pico=" << picoMB << " MB\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        uso();
        return 1;
    }

    int n = std::atoi(argv[1]);
    if (n <= 0) {
        std::cerr << "N debe ser > 0\n";
        return 1;
    }

    std::string modo = "clase_valores";
    std::string rutaCSV = "";
    for (int i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "clase_ptrs") == 0) modo = "clase_ptrs";
        else if (std::strcmp(argv[i], "struct_valores") == 0) modo = "struct_valores";
        else if (std::strcmp(argv[i], "struct_ptrs") == 0) modo = "struct_ptrs";
        else if (std::strncmp(argv[i], "--csv=", 6) == 0) rutaCSV = std::string(argv[i]+6);
    }

    std::cout << "Generando " << n << " personas en modo: " << modo << "\n";

    auto t_app0 = clk::now();
    size_t picoMB = 0;
    marca("inicio", t_app0, picoMB);

    std::srand((unsigned)std::time(nullptr));

    if (modo == "clase_valores") {
        auto t0 = clk::now();
        std::vector<Persona> v = generarColeccion(n);
        marca("post-generacion", t_app0, picoMB);

        if (!rutaCSV.empty()) { exportarCSV(v, rutaCSV); marca("post-csv", t_app0, picoMB); }

        preguntasObligatorias(v);  marca("post-obligatorias", t_app0, picoMB);
        preguntasOpcionales(v);    marca("post-opcionales",   t_app0, picoMB);

        auto tEnd = clk::now();
        double total = std::chrono::duration<double>(tEnd - t_app0).count();
        std::cout << "[Resumen] total=" << total << " s | picoRSS=" << picoMB << " MB\n";
    }
    else if (modo == "clase_ptrs") {
        std::vector<Persona*> v;
        v.reserve(n);
        for (int i = 0; i < n; ++i) {
            Persona p = generarPersona();
            v.push_back(new Persona(p));
        }
        marca("post-generacion", t_app0, picoMB);

        if (!rutaCSV.empty()) { exportarCSV(v, rutaCSV); marca("post-csv", t_app0, picoMB); }

        preguntasObligatorias(v);  marca("post-obligatorias", t_app0, picoMB);
        preguntasOpcionales(v);    marca("post-opcionales",   t_app0, picoMB);

        for (auto* p : v) delete p;
        marca("post-liberacion", t_app0, picoMB);

        auto tEnd = clk::now();
        double total = std::chrono::duration<double>(tEnd - t_app0).count();
        std::cout << "[Resumen] total=" << total << " s | picoRSS=" << picoMB << " MB\n";
    }
    else if (modo == "struct_valores") {
        std::vector<Persona> tmp = generarColeccion(n);
        std::vector<PersonaC> v; v.reserve(tmp.size());
        for (const auto& p : tmp) v.push_back(aStruct(p));
        marca("post-generacion", t_app0, picoMB);

        if (!rutaCSV.empty()) { exportarCSV(v, rutaCSV); marca("post-csv", t_app0, picoMB); }

        preguntasObligatoriasC(v); marca("post-obligatorias", t_app0, picoMB);
        preguntasOpcionalesC(v);   marca("post-opcionales",   t_app0, picoMB);

        auto tEnd = clk::now();
        double total = std::chrono::duration<double>(tEnd - t_app0).count();
        std::cout << "[Resumen] total=" << total << " s | picoRSS=" << picoMB << " MB\n";
    }
    else if (modo == "struct_ptrs") {
        std::vector<PersonaC*> v; v.reserve(n);
        for (int i=0;i<n;++i) {
            Persona p = generarPersona();
            PersonaC s = aStruct(p);
            v.push_back(new PersonaC(s));
        }
        marca("post-generacion", t_app0, picoMB);

        if (!rutaCSV.empty()) { exportarCSV(v, rutaCSV); marca("post-csv", t_app0, picoMB); }

        preguntasObligatoriasC(v); marca("post-obligatorias", t_app0, picoMB);
        preguntasOpcionalesC(v);   marca("post-opcionales",   t_app0, picoMB);

        for (auto* p : v) delete p;
        marca("post-liberacion", t_app0, picoMB);

        auto tEnd = clk::now();
        double total = std::chrono::duration<double>(tEnd - t_app0).count();
        std::cout << "[Resumen] total=" << total << " s | picoRSS=" << picoMB << " MB\n";
    } else {
        std::cerr << "Modo desconocido.\n";
        uso();
        return 1;
    }

    return 0;
}
