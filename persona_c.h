
#ifndef PERSONA_C_H
#define PERSONA_C_H

// Implementación estilo C (struct) para comparar con class
struct PersonaC {
    char nombre[64];
    char apellido[64];
    char id[32];
    char ciudad[64];
    int dia, mes, anio;
    double ingresos, patrimonio, deudas;
    bool declarante;
};

#endif // PERSONA_C_H
