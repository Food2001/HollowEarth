#include "camara.h"
#include "mundo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

Camara::Camara() {
    angulo            = 180.0f;
    anguloObjetivo    = 180.0f;
    distancia         = 25.0f;
    altura            = 15.0f;
    zoom              = 1.0f;
    zoomMin           = 0.3f;
    zoomMax           = 2.0f;
    objetivo          = glm::vec3(0.0f);
    arriba            = glm::vec3(0.0f, 1.0f, 0.0f);
    distanciaEfectiva = distancia;
    posicionActual    = getPosicion();
}

void Camara::rotarIzquierda() { anguloObjetivo += 90.0f; }
void Camara::rotarDerecha()   { anguloObjetivo -= 90.0f; }

void Camara::hacerZoom(float cantidad) {
    zoom = glm::clamp(zoom - cantidad * 0.1f, zoomMin, zoomMax);
}

glm::vec3 Camara::getPosicion() {
    float rad = glm::radians(angulo);
    return glm::vec3(
        objetivo.x + sin(rad) * distancia * zoom,
        objetivo.y + altura   * zoom,
        objetivo.z + cos(rad) * distancia * zoom
    );
}

glm::vec3 Camara::getDireccionFrente() {
    float rad = glm::radians(angulo);
    return glm::normalize(glm::vec3(-sin(rad), 0.0f, -cos(rad)));
}

// Barrido horizontal: avanza desde el jugador hacia la cámara en el plano XZ.
// En cada paso verifica si el terreno en esa posición XZ supera la altura
// que tendría la cámara si estuviera ahí.
// La cámara tiene altura fija (objetivo.y + altura*zoom), entonces
// si el terreno en algún punto del camino es más alto que eso, la cámara
// quedaría enterrada — hay que acortarla.
float Camara::calcularDistanciaLibre(Mundo& mundo, glm::vec3 posIdeal) {
    const int   PASOS       = 20;
    const float MARGEN      = 2.5f;
    const float DIST_MIN    = 4.0f;

    float rad    = glm::radians(angulo);
    float distMax = glm::length(posIdeal - objetivo);
    if (distMax < 0.001f) return distMax;

    float alturaCAMARA = objetivo.y + altura * zoom;

    for (int i = PASOS; i >= 1; i--) {
        float t    = (float)i / (float)PASOS;
        float dist = distMax * t;

        // Posición XZ de la cámara a esta distancia
        float px = objetivo.x + sin(rad) * dist;
        float pz = objetivo.z + cos(rad) * dist;

        // Altura del terreno en este punto XZ
        float alturaTerr = std::max(0.0f, mundo.obtenerAltura(px, pz)) + MARGEN;

        // Si el terreno supera la altura de la cámara → la cámara quedaría tapada
        if (alturaTerr > alturaCAMARA) {
            // Buscar el paso anterior que sea libre
            float tLibre = (float)(i - 1) / (float)PASOS;
            return std::max(distMax * tLibre, DIST_MIN);
        }
    }

    return distMax;
}

glm::mat4 Camara::getView() {
    angulo = glm::mix(angulo, anguloObjetivo, 0.1f);
    glm::vec3 posObjetivo = getPosicion();
    posicionActual = glm::mix(posicionActual, posObjetivo, 0.1f);
    return glm::lookAt(posicionActual, objetivo, arriba);
}

glm::mat4 Camara::getViewConColision(Mundo& mundo) {
    angulo = glm::mix(angulo, anguloObjetivo, 0.1f);

    float rad = glm::radians(angulo);

    // Posición ideal a distancia máxima
    glm::vec3 posIdeal = glm::vec3(
        objetivo.x + sin(rad) * distancia * zoom,
        objetivo.y + altura   * zoom,
        objetivo.z + cos(rad) * distancia * zoom
    );

    // Distancia libre según barrido
    float distLibre = calcularDistanciaLibre(mundo, posIdeal);

    // Lerp suave — acercarse rápido, alejarse más lento
    float velocidadLerp = (distLibre < distanciaEfectiva) ? 0.25f : 0.08f;
    distanciaEfectiva = glm::mix(distanciaEfectiva, distLibre, velocidadLerp);

    // Posición real
    glm::vec3 posReal = glm::vec3(
        objetivo.x + sin(rad) * distanciaEfectiva,
        objetivo.y + altura   * zoom,
        objetivo.z + cos(rad) * distanciaEfectiva
    );

    // Seguridad extra: la cámara nunca bajo del terreno
    float alturaSuelo = std::max(0.0f, mundo.obtenerAltura(posReal.x, posReal.z)) + 2.0f;
    if (posReal.y < alturaSuelo) posReal.y = alturaSuelo;

    posicionActual = glm::mix(posicionActual, posReal, 0.12f);

    return glm::lookAt(posicionActual, objetivo, arriba);
}