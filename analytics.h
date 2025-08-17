
#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <string>

class Persona;
struct PersonaC;

void preguntasObligatorias(const std::vector<Persona>& v);
void preguntasOpcionales(const std::vector<Persona>& v);

void preguntasObligatorias(const std::vector<Persona*>& v);
void preguntasOpcionales(const std::vector<Persona*>& v);

void preguntasObligatoriasC(const std::vector<PersonaC>& v);
void preguntasOpcionalesC(const std::vector<PersonaC>& v);

void preguntasObligatoriasC(const std::vector<PersonaC*>& v);
void preguntasOpcionalesC(const std::vector<PersonaC*>& v);

#endif 
