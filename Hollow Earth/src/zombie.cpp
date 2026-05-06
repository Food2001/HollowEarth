#include "zombie.h"
#include "mundo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <cstdlib>
#include <algorithm>

// ==========================================
// Geometría del cubo unitario (compartida)
// ==========================================
static float CUBO_V[] = {
    -0.5f,-0.5f, 0.5f,  0,0,1,  0,0,
     0.5f,-0.5f, 0.5f,  0,0,1,  1,0,
     0.5f, 1.0f, 0.5f,  0,0,1,  1,1,
    -0.5f, 1.0f, 0.5f,  0,0,1,  0,1,
    -0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
     0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
     0.5f, 1.0f,-0.5f,  0,0,-1, 1,1,
    -0.5f, 1.0f,-0.5f,  0,0,-1, 0,1,
    -0.5f,-0.5f,-0.5f, -1,0,0,  0,0,
    -0.5f,-0.5f, 0.5f, -1,0,0,  1,0,
    -0.5f, 1.0f, 0.5f, -1,0,0,  1,1,
    -0.5f, 1.0f,-0.5f, -1,0,0,  0,1,
     0.5f,-0.5f,-0.5f,  1,0,0,  0,0,
     0.5f,-0.5f, 0.5f,  1,0,0,  1,0,
     0.5f, 1.0f, 0.5f,  1,0,0,  1,1,
     0.5f, 1.0f,-0.5f,  1,0,0,  0,1,
    -0.5f, 1.0f,-0.5f,  0,1,0,  0,0,
     0.5f, 1.0f,-0.5f,  0,1,0,  1,0,
     0.5f, 1.0f, 0.5f,  0,1,0,  1,1,
    -0.5f, 1.0f, 0.5f,  0,1,0,  0,1,
    -0.5f,-0.5f,-0.5f,  0,-1,0, 0,0,
     0.5f,-0.5f,-0.5f,  0,-1,0, 1,0,
     0.5f,-0.5f, 0.5f,  0,-1,0, 1,1,
    -0.5f,-0.5f, 0.5f,  0,-1,0, 0,1,
};
static unsigned int CUBO_I[] = {
     0, 1, 2,  2, 3, 0,
     4, 5, 6,  6, 7, 4,
     8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20,
};

// ==========================================
// Inicialización de miembros estáticos
// ==========================================
unsigned int Zombie::sharedVAO = 0;
unsigned int Zombie::sharedVBO = 0;
unsigned int Zombie::sharedEBO = 0;
int          Zombie::sharedIndices = 36;
bool         Zombie::meshInicializada = false;
unsigned int Zombie::instanceVBO_matrices = 0;
unsigned int Zombie::instanceVBO_colores  = 0;

// ==========================================
// Inicializa la malla compartida (un solo VAO para todos los zombies)
// ==========================================
void Zombie::inicializarMallaCompartida() {
    if (meshInicializada) return;
    meshInicializada = true;

    glGenVertexArrays(1, &sharedVAO);
    glGenBuffers(1, &sharedVBO);
    glGenBuffers(1, &sharedEBO);

    glBindVertexArray(sharedVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBO_V), CUBO_V, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBO_I), CUBO_I, GL_STATIC_DRAW);

    // Atributos de vértice: posición(0), normal(1), uv(2)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Crear los VBOs para instanciado (se usarán más tarde)
    glGenBuffers(1, &instanceVBO_matrices);
    glGenBuffers(1, &instanceVBO_colores);

    glBindVertexArray(0);
}

// ==========================================
// Constructor
// ==========================================
Zombie::Zombie(glm::vec3 posicionInicial) {
    posicion           = posicionInicial;
    rotacion           = 0.0f;
    salud              = SALUD_MAX;
    estado             = EstadoZombie::IDLE;
    velocidadY         = 0.0f;
    tiempoDeambular    = 0.0f;
    dirDeambular       = glm::vec3(0.0f);
    velocidadKnockback = glm::vec3(0.0f);
    tiempoKnockback    = 0.0f;
    // Asegurar que la malla compartida exista
    inicializarMallaCompartida();
}

// ==========================================
// Recibir daño
// ==========================================
void Zombie::recibirDanio(float cantidad, glm::vec3 dirGolpe) {
    salud -= cantidad;
    salud  = std::max(0.0f, salud);

    dirGolpe.y = 0.0f;
    if (glm::length(dirGolpe) > 0.0f) {
        velocidadKnockback = glm::normalize(dirGolpe) * FUERZA_KNOCKBACK;
        tiempoKnockback    = DURACION_KNOCKBACK;
    }
}

// ==========================================
// Física básica
// ==========================================
void Zombie::actualizarFisica(float delta, Mundo& mundo) {
    velocidadY -= GRAVEDAD * delta;
    posicion.y += velocidadY * delta;

    float alturaTerreno = std::max(0.0f, mundo.obtenerAltura(posicion.x, posicion.z)) + ALTURA_SOBRE;
    if (posicion.y <= alturaTerreno) {
        posicion.y = alturaTerreno;
        velocidadY = 0.0f;
    }
}

// ==========================================
// Lógica de IA
// ==========================================
void Zombie::actualizar(float delta, glm::vec3 posJugador, Mundo& mundo) {
    if (estaMuerto()) return;

    if (tiempoKnockback > 0.0f) {
        posicion          += velocidadKnockback * delta;
        tiempoKnockback   -= delta;
        velocidadKnockback *= (1.0f - delta * 8.0f);
        actualizarFisica(delta, mundo);
        return;
    }

    glm::vec3 diff  = posJugador - posicion;
    diff.y          = 0.0f;
    float distancia = glm::length(diff);

    if (distancia <= RADIO_ATAQUE)
        estado = EstadoZombie::ATACANDO;
    else if (distancia <= RADIO_DETECCION)
        estado = EstadoZombie::PERSIGUIENDO;
    else
        estado = EstadoZombie::IDLE;

    switch (estado) {
        case EstadoZombie::IDLE: {
            tiempoDeambular -= delta;
            if (tiempoDeambular <= 0.0f) {
                tiempoDeambular = 2.0f + (rand() / (float)RAND_MAX) * 2.0f;
                float angRand   = (rand() / (float)RAND_MAX) * 360.0f;
                float rad       = glm::radians(angRand);
                dirDeambular    = glm::vec3(sin(rad), 0.0f, cos(rad));
            }
            posicion += dirDeambular * (VELOCIDAD * 0.4f) * delta;
            if (glm::length(dirDeambular) > 0.0f)
                rotacion = glm::degrees(atan2(dirDeambular.x, dirDeambular.z));
            break;
        }
        case EstadoZombie::PERSIGUIENDO: {
            glm::vec3 dir = glm::normalize(diff);
            posicion += dir * VELOCIDAD * delta;
            rotacion  = glm::degrees(atan2(dir.x, dir.z));
            break;
        }
        case EstadoZombie::ATACANDO: {
            if (distancia > 0.01f)
                rotacion = glm::degrees(atan2(diff.x, diff.z));
            break;
        }
    }

    actualizarFisica(delta, mundo);
}

// ==========================================
// Daño por segundo
// ==========================================
float Zombie::getDanioFrame(float delta) const {
    if (estado == EstadoZombie::ATACANDO && !estaMuerto() && tiempoKnockback <= 0.0f)
        return DANIO_POR_SEGUNDO * delta;
    return 0.0f;
}

// ==========================================
// Dibujado individual (usa el shader externo ya configurado)
// ==========================================
void Zombie::dibujar() {
    glBindVertexArray(sharedVAO);
    glDrawElements(GL_TRIANGLES, sharedIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ==========================================
// Dibujado instanciado (opcional, gran mejora de FPS)
// ==========================================
void Zombie::dibujarInstanciado(const std::vector<glm::mat4>& matrices,
                                const std::vector<glm::vec3>& colores,
                                Shader& shader) {
    if (matrices.empty()) return;
    if (!meshInicializada) inicializarMallaCompartida();

    // Bandera estática para configurar los atributos de instancia una sola vez
    static bool instanceAttribsConfigured = false;

    glBindVertexArray(sharedVAO);

    // Subir datos de matrices y colores
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_matrices);
    glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), matrices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_colores);
    glBufferData(GL_ARRAY_BUFFER, colores.size() * sizeof(glm::vec3), colores.data(), GL_DYNAMIC_DRAW);

    // Configurar los atributos de instancia solo la primera vez
    if (!instanceAttribsConfigured) {
        instanceAttribsConfigured = true;

        // Matriz 4x4 en locations 3,4,5,6
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_matrices);
        size_t vec4Size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
            glVertexAttribDivisor(3 + i, 1);
        }

        // Color (vec3) en location 7
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO_colores);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glVertexAttribDivisor(7, 1);
    }

    shader.setBool("usarInstancia", true);
    shader.setBool("usarTextura", false);

    glDrawElementsInstanced(GL_TRIANGLES, sharedIndices, GL_UNSIGNED_INT, 0, (GLsizei)matrices.size());

    glBindVertexArray(0);
    shader.setBool("usarInstancia", false);
}