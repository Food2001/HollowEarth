#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "inventario.h"

class Mundo;

class Jugador {
public:
    glm::vec3 posicion;
    float     velocidad;
    float     velocidadCorrer;
    float     rotacion;
    float     velocidadY;
    bool      enSuelo;
    bool      corriendo;

    // Stamina
    float stamina;
    bool  agotado;
    float tiempoAgotado;

    // Ataque
    float cooldownAtaque;
    bool  atacando;
    float tiempoAnimAtaque;

    // Inventario
    Inventario inventario;

    // Constantes de física
    static constexpr float GRAVEDAD         = 9.8f;
    static constexpr float ALTURA_SOBRE     = 0.3f;
    static constexpr float FUERZA_SALTO     = 5.0f;
    static constexpr float HISTERESIS_SUELO = 0.1f;

    // Constantes de stamina
    static constexpr float STAMINA_MAX             = 100.0f;
    static constexpr float STAMINA_CONSUMO         = 12.0f;
    static constexpr float STAMINA_RECARGA         = 10.0f;
    static constexpr float STAMINA_UMBRAL_RECOVERY = 25.0f;
    static constexpr float DURACION_AGOTAMIENTO    = 2.5f;
    static constexpr float VELOCIDAD_AGOTADO       = 2.5f;

    // Constantes de ataque
    static constexpr float COOLDOWN_ATAQUE = 0.5f;
    static constexpr float ALCANCE_ATAQUE  = 2.5f;
    static constexpr float ANGULO_CONO     = 90.0f;
    static constexpr float DANIO_ATAQUE    = 34.0f;
    static constexpr float DURACION_ANIM   = 0.15f;

    unsigned int VAO, VBO, EBO;

    Jugador();
    ~Jugador();

    void mover(glm::vec3 direccion, float delta, bool correr);
    void saltar();
    void actualizarFisica(float delta, Mundo& mundo);
    void actualizarStamina(float delta, bool intentaCorrer);
    void actualizarAtaque(float delta);
    bool intentarAtacar();

    // Usar item seleccionado — devuelve cuánta salud recuperar (0 si nada)
    float usarItemSeleccionado();

    float getStaminaPorcentaje() const { return stamina / STAMINA_MAX; }
    bool  estaAgotado()          const { return agotado; }
    bool  estaAtacando()         const { return tiempoAnimAtaque > 0.0f; }

    // Velocidad afectada por peso del inventario
    float getVelocidadEfectiva() const;

    void dibujar();
};