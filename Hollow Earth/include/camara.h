#pragma once
#include <glm/glm.hpp>

class Mundo;

// ================================================
// Camara orbital — siempre mira al objetivo
// Q/E rotan, rueda del mouse hace zoom
// Se acerca automáticamente si hay terreno en medio
// ================================================
class Camara {
public:
    float angulo;
    float anguloObjetivo;
    float distancia;      // distancia base
    float altura;
    float zoom;
    float zoomMin;
    float zoomMax;

    glm::vec3 objetivo;
    glm::vec3 arriba;

    Camara();

    void rotarIzquierda();
    void rotarDerecha();
    void hacerZoom(float cantidad);

    glm::mat4 getView();
    glm::mat4 getViewConColision(Mundo& mundo);
    glm::vec3 getPosicion();
    glm::vec3 getDireccionFrente();

private:
    glm::vec3 posicionActual;

    // Distancia real después de aplicar colisión con terreno
    float distanciaEfectiva;

    // Raycast: devuelve la distancia máxima libre entre objetivo y posición ideal
    float calcularDistanciaLibre(Mundo& mundo, glm::vec3 posIdeal);
};