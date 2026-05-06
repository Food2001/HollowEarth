#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"

class Mundo;

enum class EstadoZombie {
    IDLE,
    PERSIGUIENDO,
    ATACANDO
};

class Zombie {
public:
    glm::vec3    posicion;
    float        rotacion;
    float        salud;
    EstadoZombie estado;
    float        velocidadY;

    // Knockback
    glm::vec3 velocidadKnockback;
    float     tiempoKnockback;

    static constexpr float RADIO_DETECCION    = 20.0f;
    static constexpr float RADIO_ATAQUE       = 1.5f;
    static constexpr float VELOCIDAD          = 2.5f;
    static constexpr float DANIO_POR_SEGUNDO  = 8.0f;
    static constexpr float SALUD_MAX          = 100.0f;
    static constexpr float ALTURA_SOBRE       = 0.3f;
    static constexpr float GRAVEDAD           = 9.8f;
    static constexpr float FUERZA_KNOCKBACK   = 6.0f;
    static constexpr float DURACION_KNOCKBACK = 0.2f;

    Zombie(glm::vec3 posicionInicial);
    ~Zombie() = default;

    void  actualizar(float delta, glm::vec3 posJugador, Mundo& mundo);
    void  recibirDanio(float cantidad, glm::vec3 dirGolpe);
    void  dibujar();   // dibuja este zombie (usa la malla compartida)

    bool  estaMuerto()              const { return salud <= 0.0f; }
    float getSaludPorcentaje()      const { return salud / SALUD_MAX; }
    float getDanioFrame(float delta) const;

    void  actualizarFisica(float delta, Mundo& mundo);

    // ---- NUEVO: sistema de malla compartida e instanciado ----
    static void inicializarMallaCompartida();   // llamar una vez al inicio
    static void dibujarInstanciado(const std::vector<glm::mat4>& matrices,
                                   const std::vector<glm::vec3>& colores,
                                   Shader& shader);

private:
    float     tiempoDeambular;
    glm::vec3 dirDeambular;

    // Miembros estáticos para la malla común (un solo VAO para todos los zombies)
    static unsigned int sharedVAO;
    static unsigned int sharedVBO;
    static unsigned int sharedEBO;
    static int          sharedIndices;
    static bool         meshInicializada;

    // VBOs para el instanciado
    static unsigned int instanceVBO_matrices;
    static unsigned int instanceVBO_colores;
};