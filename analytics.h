
#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <string>

class Persona;
struct PersonaC;

// Preguntas obligatorias y opcionales para vector de objetos (valores)
void preguntasObligatorias(const std::vector<Persona>& v);
void preguntasOpcionales(const std::vector<Persona>& v);

// Versión para punteros
void preguntasObligatorias(const std::vector<Persona*>& v);
void preguntasOpcionales(const std::vector<Persona*>& v);

// Versión para struct "C"
void preguntasObligatoriasC(const std::vector<PersonaC>& v);
void preguntasOpcionalesC(const std::vector<PersonaC>& v);

// Versión para struct "C" punteros
void preguntasObligatoriasC(const std::vector<PersonaC*>& v);
void preguntasOpcionalesC(const std::vector<PersonaC*>& v);

#endif // ANALYTICS_H
