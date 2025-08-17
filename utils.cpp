
#include "utils.h"
#include "persona.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

Fecha parseFecha(const std::string& fecha) {
    Fecha f{1,1,2000};
    char sep1='/', sep2='/';
    std::stringstream ss(fecha);
    ss >> f.dia >> sep1 >> f.mes >> sep2 >> f.anio;
    if (f.dia < 1) f.dia = 1;
    if (f.mes < 1) f.mes = 1;
    if (f.anio < 1900) f.anio = 1900;
    return f;
}

int calcularEdad(const std::string& fechaNacimiento) {
    Fecha f = parseFecha(fechaNacimiento);
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int anio = now->tm_year + 1900;
    int mes  = now->tm_mon + 1;
    int dia  = now->tm_mday;

    int edad = anio - f.anio;
    if (mes < f.mes || (mes == f.mes && dia < f.dia)) {
        edad -= 1;
    }
    if (edad < 0) edad = 0;
    return edad;
}

int ultimosDosDigitos(const std::string& id) {
    if (id.empty()) return 0;
    int n = (int)id.size();
    int d1 = -1, d0 = -1; 
    for (int i = n-1; i >= 0; --i) {
        if (id[i] >= '0' && id[i] <= '9') {
            if (d0 == -1) d0 = id[i] - '0';
            else if (d1 == -1) { d1 = id[i] - '0'; break; }
        }
    }
    if (d0 == -1) return 0;
    if (d1 == -1) return d0; 
    return d1*10 + d0;
}

char grupoCalendarioPorDosDigitos(int dosDigitos) {
    if (dosDigitos <= 39) return 'A';
    if (dosDigitos <= 79) return 'B';
    return 'C';
}

std::string etiquetaRango20(int d) {
    if (d < 0) d = 0;
    if (d > 99) d = 99;
    int base = (d/20)*20;          
    int to   = std::min(base+19,99);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << base << "-" 
        << std::setw(2) << std::setfill('0') << to;
    return oss.str();
}

size_t memoriaRSS_MB() {
    std::ifstream in("/proc/self/status");
    if (!in.is_open()) return 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream ss(line);
            std::string key, kbStr, unit;
            size_t kb = 0;
            ss >> key >> kb >> unit;
            return kb / 1024;
        }
    }
    return 0;
}

static void escribirCabecera(std::ofstream& out) {
    out << "id,nombre,apellido,ciudad,fecha,ingresos,patrimonio,deudas,declarante\n";
}

void exportarCSV(const std::vector<Persona>& v, const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << ruta << " para escribir.\n";
        return;
    }
    escribirCabecera(out);
    for (const auto& p : v) {
        out << p.getId() << ","
            << p.getNombre() << ","
            << p.getApellido() << ","
            << p.getCiudadNacimiento() << ","
            << p.getFechaNacimiento() << ","
            << p.getIngresosAnuales() << ","
            << p.getPatrimonio() << ","
            << p.getDeudas() << ","
            << (p.getDeclaranteRenta() ? "1" : "0") << "\n";
    }
}

void exportarCSV(const std::vector<Persona*>& v, const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << ruta << " para escribir.\n";
        return;
    }
    escribirCabecera(out);
    for (const auto* p : v) {
        if (!p) continue;
        out << p->getId() << ","
            << p->getNombre() << ","
            << p->getApellido() << ","
            << p->getCiudadNacimiento() << ","
            << p->getFechaNacimiento() << ","
            << p->getIngresosAnuales() << ","
            << p->getPatrimonio() << ","
            << p->getDeudas() << ","
            << (p->getDeclaranteRenta() ? "1" : "0") << "\n";
    }
}

struct PersonaC {
    char nombre[64];
    char apellido[64];
    char id[32];
    char ciudad[64];
    int dia, mes, anio;
    double ingresos, patrimonio, deudas;
    bool declarante;
};

void exportarCSV(const std::vector<PersonaC>& v, const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << ruta << " para escribir.\n";
        return;
    }
    escribirCabecera(out);
    for (const auto& p : v) {
        std::ostringstream f;
        f << p.dia << "/" << p.mes << "/" << p.anio;
        out << p.id << ","
            << p.nombre << ","
            << p.apellido << ","
            << p.ciudad << ","
            << f.str() << ","
            << p.ingresos << ","
            << p.patrimonio << ","
            << p.deudas << ","
            << (p.declarante ? "1" : "0") << "\n";
    }
}

void exportarCSV(const std::vector<PersonaC*>& v, const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) {
        std::cerr << "No se pudo abrir " << ruta << " para escribir.\n";
        return;
    }
    escribirCabecera(out);
    for (const auto* p : v) {
        if (!p) continue;
        std::ostringstream f;
        f << p->dia << "/" << p->mes << "/" << p->anio;
        out << p->id << ","
            << p->nombre << ","
            << p->apellido << ","
            << p->ciudad << ","
            << f.str() << ","
            << p->ingresos << ","
            << p->patrimonio << ","
            << p->deudas << ","
            << (p->declarante ? "1" : "0") << "\n";
    }
}
