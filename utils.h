
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <iostream>

// Estructura simple para manejar fechas
struct Fecha {
    int dia;
    int mes;
    int anio;
};

// --- Utilidades generales ---
Fecha parseFecha(const std::string& fecha);                  // "DD/MM/AAAA" o "D/M/AAAA"
int calcularEdad(const std::string& fechaNacimiento);        // Edad al día de hoy
int ultimosDosDigitos(const std::string& id);                // 0..99
char grupoCalendarioPorDosDigitos(int dosDigitos);           // 'A','B','C'
std::string etiquetaRango20(int dosDigitos);                 // "00-19","20-39",...

// Memoria residente en MB (Linux). Si no se puede leer, retorna 0.
size_t memoriaRSS_MB();

// Exportar CSV de personas (clase, punteros y struct)
class Persona; // forward declaration para no incluir persona.h aquí

void exportarCSV(const std::vector<Persona>& v, const std::string& ruta);
void exportarCSV(const std::vector<Persona*>& v, const std::string& ruta);

// Versión para struct "C"
struct PersonaC;
void exportarCSV(const std::vector<PersonaC>& v, const std::string& ruta);
void exportarCSV(const std::vector<PersonaC*>& v, const std::string& ruta);

#endif // UTILS_H
