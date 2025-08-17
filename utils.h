
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <iostream>

struct Fecha {
    int dia;
    int mes;
    int anio;
};

Fecha parseFecha(const std::string& fecha);                
int calcularEdad(const std::string& fechaNacimiento);     
int ultimosDosDigitos(const std::string& id);              
char grupoCalendarioPorDosDigitos(int dosDigitos);    
std::string etiquetaRango20(int dosDigitos);               
size_t memoriaRSS_MB();

class Persona; 
void exportarCSV(const std::vector<Persona>& v, const std::string& ruta);
void exportarCSV(const std::vector<Persona*>& v, const std::string& ruta);

struct PersonaC;
void exportarCSV(const std::vector<PersonaC>& v, const std::string& ruta);
void exportarCSV(const std::vector<PersonaC*>& v, const std::string& ruta);

#endif 
