
#include "analytics.h"
#include "persona.h"
#include "persona_c.h"
#include "utils.h"

#include <iostream>
#include <map>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <sstream>

// Helpers comunes
static char grupoDeID(const std::string& id) {
    int d = ultimosDosDigitos(id);
    return grupoCalendarioPorDosDigitos(d);
}

static void imprimirPersonaMinima(const Persona* p, const std::string& pref) {
    if (!p) return;
    std::cout << pref << " [" << p->getId() << "] " << p->getNombre() << " " << p->getApellido()
              << " | " << p->getCiudadNacimiento() << " | nac: " << p->getFechaNacimiento()
              << " | patr: " << p->getPatrimonio() << "\n";
}

// --- CLASE: valores ---
void preguntasObligatorias(const std::vector<Persona>& v) {
    if (v.empty()) { std::cout << "No hay datos.\n"; return; }

    // 1. Persona más longeva (global y por ciudad)
    const Persona* masLongeva = nullptr;
    std::unordered_map<std::string,const Persona*> longevaPorCiudad;

    for (const auto& p : v) {
        if (!masLongeva || calcularEdad(p.getFechaNacimiento()) > calcularEdad(masLongeva->getFechaNacimiento())) {
            masLongeva = &p;
        }
        auto& ptr = longevaPorCiudad[p.getCiudadNacimiento()];
        if (!ptr || calcularEdad(p.getFechaNacimiento()) > calcularEdad(ptr->getFechaNacimiento())) {
            ptr = &p;
        }
    }

    std::cout << "\n[1] Persona más longeva (global):\n";
    imprimirPersonaMinima(masLongeva, "   ");

    std::cout << "[1] Persona más longeva por ciudad:\n";
    for (auto& kv : longevaPorCiudad) {
        imprimirPersonaMinima(kv.second, "   " + kv.first + ": ");
    }

    // 2. Mayor patrimonio (país, por ciudad, por grupo)
    const Persona* masRica = nullptr;
    std::unordered_map<std::string,const Persona*> ricaPorCiudad;
    std::unordered_map<char,const Persona*> ricaPorGrupo;

    for (const auto& p : v) {
        if (!masRica || p.getPatrimonio() > masRica->getPatrimonio())
            masRica = &p;

        auto& rc = ricaPorCiudad[p.getCiudadNacimiento()];
        if (!rc || p.getPatrimonio() > rc->getPatrimonio())
            rc = &p;

        char g = grupoDeID(p.getId());
        auto& rg = ricaPorGrupo[g];
        if (!rg || p.getPatrimonio() > rg->getPatrimonio())
            rg = &p;
    }

    std::cout << "\n[2] Persona con mayor patrimonio (global):\n";
    imprimirPersonaMinima(masRica, "   ");

    std::cout << "[2] Persona con mayor patrimonio por ciudad:\n";
    for (auto& kv : ricaPorCiudad) imprimirPersonaMinima(kv.second, "   " + kv.first + ": ");

    std::cout << "[2] Persona con mayor patrimonio por grupo (A/B/C):\n";
    for (auto& kv : ricaPorGrupo) {
        std::string pref = std::string("   Grupo ") + kv.first + ": ";
        imprimirPersonaMinima(kv.second, pref);
    }

    // 3. Declarantes por calendario. Validar asignación según dos dígitos
    std::map<char,int> conteoGrupo; // A,B,C
    std::map<char,int> declarantesGrupo; // solo p.getDeclaranteRenta()==true
    bool todoValido = true; // aquí validamos que el grupo calculado cae en A,B,C (siempre debería)
    for (const auto& p : v) {
        char g = grupoDeID(p.getId());
        if (g!='A' && g!='B' && g!='C') todoValido = false;
        conteoGrupo[g]++;
        if (p.getDeclaranteRenta()) declarantesGrupo[g]++;
    }

    std::cout << "\n[3] Conteo por calendario (A/B/C) y declarantes:\n";
    for (auto g : {'A','B','C'}) {
        std::cout << "   Grupo " << g << ": total=" << conteoGrupo[g]
                  << ", declarantes=" << declarantesGrupo[g] << "\n";
    }
    std::cout << "   Validación de asignación por terminación: " << (todoValido ? "OK" : "Revisar") << "\n";
}

void preguntasOpcionales(const std::vector<Persona>& v) {
    if (v.empty()) return;

    // a) Ciudad con mayor número de personas
    std::unordered_map<std::string,int> cuentaCiudad;
    for (const auto& p : v) cuentaCiudad[p.getCiudadNacimiento()]++;
    std::string mayorCiudad = "N/A";
    int maxCant = -1;
    for (auto& kv : cuentaCiudad) {
        if (kv.second > maxCant) { maxCant = kv.second; mayorCiudad = kv.first; }
    }
    std::cout << "\n[Extra 1] Ciudad con más personas: " << mayorCiudad << " (" << maxCant << ")\n";

    // b) Menores de 18 total y por ciudad
    int menoresTotal = 0;
    std::unordered_map<std::string,int> menoresCiudad;
    for (const auto& p : v) {
        if (calcularEdad(p.getFechaNacimiento()) < 18) {
            menoresTotal++;
            menoresCiudad[p.getCiudadNacimiento()]++;
        }
    }
    std::cout << "[Extra 2] Menores de 18: total=" << menoresTotal << "\n";
    for (auto& kv : menoresCiudad) {
        std::cout << "   " << kv.first << ": " << kv.second << "\n";
    }

    // c) Rango 20 más frecuente según terminación del documento
    std::unordered_map<std::string,int> conteoRangos;
    for (const auto& p : v) {
        int d = ultimosDosDigitos(p.getId());
        std::string etiqueta = etiquetaRango20(d);
        conteoRangos[etiqueta]++;
    }
    std::string mejor = "00-19"; int best = -1;
    for (auto& kv : conteoRangos) if (kv.second > best) { best = kv.second; mejor = kv.first; }
    std::cout << "[Extra 3] Rango de documento más frecuente: " << mejor << " (" << best << ")\n";
}

// --- CLASE: punteros ---
void preguntasObligatorias(const std::vector<Persona*>& v) {
    if (v.empty()) { std::cout << "No hay datos.\n"; return; }

    const Persona* masLongeva = nullptr;
    std::unordered_map<std::string,const Persona*> longevaPorCiudad;

    for (const auto* p : v) {
        if (!p) continue;
        if (!masLongeva || calcularEdad(p->getFechaNacimiento()) > calcularEdad(masLongeva->getFechaNacimiento())) {
            masLongeva = p;
        }
        auto& ptr = longevaPorCiudad[p->getCiudadNacimiento()];
        if (!ptr || calcularEdad(p->getFechaNacimiento()) > calcularEdad(ptr->getFechaNacimiento())) {
            ptr = p;
        }
    }

    std::cout << "\n[1] Persona más longeva (global):\n";
    imprimirPersonaMinima(masLongeva, "   ");

    std::cout << "[1] Persona más longeva por ciudad:\n";
    for (auto& kv : longevaPorCiudad) imprimirPersonaMinima(kv.second, "   " + kv.first + ": ");

    const Persona* masRica = nullptr;
    std::unordered_map<std::string,const Persona*> ricaPorCiudad;
    std::unordered_map<char,const Persona*> ricaPorGrupo;

    for (const auto* p : v) {
        if (!p) continue;
        if (!masRica || p->getPatrimonio() > masRica->getPatrimonio()) masRica = p;
        auto& rc = ricaPorCiudad[p->getCiudadNacimiento()];
        if (!rc || p->getPatrimonio() > rc->getPatrimonio()) rc = p;
        char g = grupoDeID(p->getId());
        auto& rg = ricaPorGrupo[g];
        if (!rg || p->getPatrimonio() > rg->getPatrimonio()) rg = p;
    }

    std::cout << "\n[2] Persona con mayor patrimonio (global):\n";
    imprimirPersonaMinima(masRica, "   ");
    std::cout << "[2] Persona con mayor patrimonio por ciudad:\n";
    for (auto& kv : ricaPorCiudad) imprimirPersonaMinima(kv.second, "   " + kv.first + ": ");
    std::cout << "[2] Persona con mayor patrimonio por grupo (A/B/C):\n";
    for (auto& kv : ricaPorGrupo) imprimirPersonaMinima(kv.second, std::string("   Grupo ") + kv.first + ": ");

    std::map<char,int> conteoGrupo, declarantesGrupo;
    bool todoValido = true;
    for (const auto* p : v) {
        if (!p) continue;
        char g = grupoDeID(p->getId());
        if (g!='A' && g!='B' && g!='C') todoValido = false;
        conteoGrupo[g]++;
        if (p->getDeclaranteRenta()) declarantesGrupo[g]++;
    }
    std::cout << "\n[3] Conteo por calendario (A/B/C) y declarantes:\n";
    for (auto g : {'A','B','C'}) {
        std::cout << "   Grupo " << g << ": total=" << conteoGrupo[g]
                  << ", declarantes=" << declarantesGrupo[g] << "\n";
    }
    std::cout << "   Validación de asignación por terminación: " << (todoValido ? "OK" : "Revisar") << "\n";
}

void preguntasOpcionales(const std::vector<Persona*>& v) {
    if (v.empty()) return;
    std::unordered_map<std::string,int> cuentaCiudad;
    for (const auto* p : v) { if (p) cuentaCiudad[p->getCiudadNacimiento()]++; }
    std::string mayorCiudad="N/A"; int maxCant=-1;
    for (auto& kv : cuentaCiudad) if (kv.second > maxCant) { maxCant=kv.second; mayorCiudad=kv.first; }
    std::cout << "\n[Extra 1] Ciudad con más personas: " << mayorCiudad << " (" << maxCant << ")\n";

    int menoresTotal=0; std::unordered_map<std::string,int> menoresCiudad;
    for (const auto* p : v) if (p && calcularEdad(p->getFechaNacimiento())<18) {
        menoresTotal++; menoresCiudad[p->getCiudadNacimiento()]++;
    }
    std::cout << "[Extra 2] Menores de 18: total=" << menoresTotal << "\n";
    for (auto& kv : menoresCiudad) std::cout << "   " << kv.first << ": " << kv.second << "\n";

    std::unordered_map<std::string,int> conteoRangos;
    for (const auto* p : v) if (p) {
        int d = ultimosDosDigitos(p->getId());
        conteoRangos[etiquetaRango20(d)]++;
    }
    std::string mejor="00-19"; int best=-1;
    for (auto& kv : conteoRangos) if (kv.second>best) { best=kv.second; mejor=kv.first; }
    std::cout << "[Extra 3] Rango de documento más frecuente: " << mejor << " (" << best << ")\n";
}

// --- STRUCT C: valores ---
static void imprimirPersonaC(const PersonaC* p, const std::string& pref) {
    if (!p) return;
    std::cout << pref << " [" << p->id << "] " << p->nombre << " " << p->apellido
              << " | " << p->ciudad << " | nac: " << p->dia << "/" << p->mes << "/" << p->anio
              << " | patr: " << p->patrimonio << "\n";
}

static int edadC(const PersonaC& p) {
    std::ostringstream f; f << p.dia << "/" << p.mes << "/" << p.anio;
    return calcularEdad(f.str());
}

static char grupoC(const PersonaC& p) {
    // Tomar últimos dos dígitos de p.id (string estilo C)
    std::string sid(p.id);
    return grupoCalendarioPorDosDigitos(ultimosDosDigitos(sid));
}

void preguntasObligatoriasC(const std::vector<PersonaC>& v) {
    if (v.empty()) { std::cout << "No hay datos.\n"; return; }

    const PersonaC* masLongeva = nullptr;
    std::unordered_map<std::string,const PersonaC*> longevaPorCiudad;
    for (const auto& p : v) {
        if (!masLongeva || edadC(p) > edadC(*masLongeva)) masLongeva = &p;
        auto& ref = longevaPorCiudad[p.ciudad];
        if (!ref || edadC(p) > edadC(*ref)) ref = &p;
    }
    std::cout << "\n[1] Persona más longeva (global):\n"; imprimirPersonaC(masLongeva, "   ");
    std::cout << "[1] Persona más longeva por ciudad:\n"; for (auto& kv : longevaPorCiudad) imprimirPersonaC(kv.second, "   " + kv.first + ": ");

    const PersonaC* masRica = nullptr;
    std::unordered_map<std::string,const PersonaC*> ricaPorCiudad;
    std::unordered_map<char,const PersonaC*> ricaPorGrupo;
    for (const auto& p : v) {
        if (!masRica || p.patrimonio > masRica->patrimonio) masRica = &p;
        auto& rc = ricaPorCiudad[p.ciudad];
        if (!rc || p.patrimonio > rc->patrimonio) rc = &p;
        char g = grupoC(p);
        auto& rg = ricaPorGrupo[g];
        if (!rg || p.patrimonio > rg->patrimonio) rg = &p;
    }
    std::cout << "\n[2] Persona con mayor patrimonio (global):\n"; imprimirPersonaC(masRica, "   ");
    std::cout << "[2] Persona con mayor patrimonio por ciudad:\n"; for (auto& kv : ricaPorCiudad) imprimirPersonaC(kv.second, "   " + kv.first + ": ");
    std::cout << "[2] Persona con mayor patrimonio por grupo (A/B/C):\n"; for (auto& kv : ricaPorGrupo) imprimirPersonaC(kv.second, std::string("   Grupo ") + kv.first + ": ");

    std::map<char,int> conteoGrupo, declarantesGrupo;
    bool todoValido = true;
    for (const auto& p : v) {
        char g = grupoC(p);
        if (g!='A' && g!='B' && g!='C') todoValido = false;
        conteoGrupo[g]++;
        if (p.declarante) declarantesGrupo[g]++;
    }
    std::cout << "\n[3] Conteo por calendario (A/B/C) y declarantes:\n";
    for (auto g : {'A','B','C'}) {
        std::cout << "   Grupo " << g << ": total=" << conteoGrupo[g]
                  << ", declarantes=" << declarantesGrupo[g] << "\n";
    }
    std::cout << "   Validación de asignación por terminación: " << (todoValido ? "OK" : "Revisar") << "\n";
}

void preguntasOpcionalesC(const std::vector<PersonaC>& v) {
    if (v.empty()) return;
    std::unordered_map<std::string,int> cuentaCiudad;
    for (const auto& p : v) cuentaCiudad[p.ciudad]++;
    std::string mayorCiudad="N/A"; int maxCant=-1;
    for (auto& kv : cuentaCiudad) if (kv.second>maxCant) {maxCant=kv.second; mayorCiudad=kv.first;}
    std::cout << "\n[Extra 1] Ciudad con más personas: " << mayorCiudad << " (" << maxCant << ")\n";

    int menoresTotal=0; std::unordered_map<std::string,int> menoresCiudad;
    for (const auto& p : v) if (edadC(p)<18) { menoresTotal++; menoresCiudad[p.ciudad]++; }
    std::cout << "[Extra 2] Menores de 18: total=" << menoresTotal << "\n";
    for (auto& kv : menoresCiudad) std::cout << "   " << kv.first << ": " << kv.second << "\n";

    std::unordered_map<std::string,int> conteoRangos;
    for (const auto& p : v) {
        std::string id(p.id);
        int d = ultimosDosDigitos(id);
        conteoRangos[etiquetaRango20(d)]++;
    }
    std::string mejor="00-19"; int best=-1;
    for (auto& kv : conteoRangos) if (kv.second>best) {best=kv.second; mejor=kv.first;}
    std::cout << "[Extra 3] Rango de documento más frecuente: " << mejor << " (" << best << ")\n";
}

// --- STRUCT C: punteros ---
void preguntasObligatoriasC(const std::vector<PersonaC*>& v) {
    if (v.empty()) { std::cout << "No hay datos.\n"; return; }

    const PersonaC* masLongeva=nullptr;
    std::unordered_map<std::string,const PersonaC*> longevaPorCiudad;
    for (auto* p : v) {
        if (!p) continue;
        if (!masLongeva || edadC(*p) > edadC(*masLongeva)) masLongeva = p;
        auto& ref = longevaPorCiudad[p->ciudad];
        if (!ref || edadC(*p) > edadC(*ref)) ref = p;
    }
    std::cout << "\n[1] Persona más longeva (global):\n"; imprimirPersonaC(masLongeva, "   ");
    std::cout << "[1] Persona más longeva por ciudad:\n"; for (auto& kv : longevaPorCiudad) imprimirPersonaC(kv.second, "   " + kv.first + ": ");

    const PersonaC* masRica=nullptr;
    std::unordered_map<std::string,const PersonaC*> ricaPorCiudad;
    std::unordered_map<char,const PersonaC*> ricaPorGrupo;
    for (auto* p : v) {
        if (!p) continue;
        if (!masRica || p->patrimonio > masRica->patrimonio) masRica = p;
        auto& rc = ricaPorCiudad[p->ciudad];
        if (!rc || p->patrimonio > rc->patrimonio) rc = p;
        char g = grupoC(*p);
        auto& rg = ricaPorGrupo[g];
        if (!rg || p->patrimonio > rg->patrimonio) rg = p;
    }
    std::cout << "\n[2] Persona con mayor patrimonio (global):\n"; imprimirPersonaC(masRica, "   ");
    std::cout << "[2] Persona con mayor patrimonio por ciudad:\n"; for (auto& kv : ricaPorCiudad) imprimirPersonaC(kv.second, "   " + kv.first + ": ");
    std::cout << "[2] Persona con mayor patrimonio por grupo (A/B/C):\n"; for (auto& kv : ricaPorGrupo) imprimirPersonaC(kv.second, std::string("   Grupo ") + kv.first + ": ");

    std::map<char,int> conteoGrupo, declarantesGrupo;
    bool todoValido=true;
    for (auto* p : v) {
        if (!p) continue;
        char g = grupoC(*p);
        if (g!='A' && g!='B' && g!='C') todoValido = false;
        conteoGrupo[g]++;
        if (p->declarante) declarantesGrupo[g]++;
    }
    std::cout << "\n[3] Conteo por calendario (A/B/C) y declarantes:\n";
    for (auto g : {'A','B','C'}) {
        std::cout << "   Grupo " << g << ": total=" << conteoGrupo[g]
                  << ", declarantes=" << declarantesGrupo[g] << "\n";
    }
    std::cout << "   Validación de asignación por terminación: " << (todoValido ? "OK" : "Revisar") << "\n";
}

void preguntasOpcionalesC(const std::vector<PersonaC*>& v) {
    if (v.empty()) return;
    std::unordered_map<std::string,int> cuentaCiudad;
    for (auto* p : v) if (p) cuentaCiudad[p->ciudad]++;
    std::string mayorCiudad="N/A"; int maxCant=-1;
    for (auto& kv : cuentaCiudad) if (kv.second>maxCant) {maxCant=kv.second; mayorCiudad=kv.first;}
    std::cout << "\n[Extra 1] Ciudad con más personas: " << mayorCiudad << " (" << maxCant << ")\n";

    int menoresTotal=0; std::unordered_map<std::string,int> menoresCiudad;
    for (auto* p : v) if (p && edadC(*p) < 18) { menoresTotal++; menoresCiudad[p->ciudad]++; }
    std::cout << "[Extra 2] Menores de 18: total=" << menoresTotal << "\n";
    for (auto& kv : menoresCiudad) std::cout << "   " << kv.first << ": " << kv.second << "\n";

    std::unordered_map<std::string,int> conteoRangos;
    for (auto* p : v) if (p) {
        std::string id(p->id);
        int d = ultimosDosDigitos(id);
        conteoRangos[etiquetaRango20(d)]++;
    }
    std::string mejor="00-19"; int best=-1;
    for (auto& kv : conteoRangos) if (kv.second>best) {best=kv.second; mejor=kv.first;}
    std::cout << "[Extra 3] Rango de documento más frecuente: " << mejor << " (" << best << ")\n";
}
