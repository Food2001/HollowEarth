#include "gestor.h"
#include <algorithm>
#include <numeric>

Gestor::Gestor() : frameActual(0) {}

void Gestor::iniciarFrame() {
    frameActual++;
    resetStats();
}

bool Gestor::debeActualizar(glm::vec3 posEntidad, glm::vec3 posJugador, int indice) const {
    float distancia = glm::length(posEntidad - posJugador);

    if (distancia <= RADIO_CERCANO) {
        return true; // cada frame
    } else if (distancia <= RADIO_MEDIO) {
        return (frameActual % FREQ_MEDIA) == (indice % FREQ_MEDIA);
    } else {
        return (frameActual % FREQ_LEJANA) == (indice % FREQ_LEJANA);
    }
}

bool Gestor::debeActualizarGlobal(int indice, int frecuencia) const {
    return (frameActual % frecuencia) == (indice % frecuencia);
}

void Gestor::registrarFPS(float fps) {
    historialFPS.push_back(fps);
    if ((int)historialFPS.size() > MAX_HISTORIAL) {
        historialFPS.pop_front();
    }
}

float Gestor::getFPSPromedio() const {
    if (historialFPS.empty()) return 0.0f;
    float suma = 0.0f;
    for (float f : historialFPS) suma += f;
    return suma / (float)historialFPS.size();
}

float Gestor::getFPSMinimo() const {
    if (historialFPS.empty()) return 0.0f;
    return *std::min_element(historialFPS.begin(), historialFPS.end());
}

float Gestor::getFPSMaximo() const {
    if (historialFPS.empty()) return 0.0f;
    return *std::max_element(historialFPS.begin(), historialFPS.end());
}

void Gestor::registrarDelta(float delta, int zombies, int chunks) {
    if (delta > UMBRAL_TIRON) {
        if ((int)tirones.size() >= MAX_TIRONES) {
            tirones.erase(tirones.begin());
        }
        tirones.push_back({delta, zombies, chunks, frameActual});
    }
}
void Gestor::registrarTiempoZombies(float tiempo) {
    tiemposZombies.push_back(tiempo);
    if ((int)tiemposZombies.size() > MAX_TIEMPOS_ZOMBIES) {
        tiemposZombies.pop_front();
    }
}

float Gestor::getTiempoPromedioZombies() const {
    if (tiemposZombies.empty()) return 0.0f;
    float suma = 0.0f;
    for (float t : tiemposZombies) suma += t;
    return suma / (float)tiemposZombies.size();
}