#include "jugador.h"
#include "mundo.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

Jugador::Jugador() {
    posicion        = glm::vec3(0.0f, 10.0f, 0.0f);
    velocidad       = 5.0f;
    velocidadCorrer = 10.0f;
    rotacion        = 0.0f;
    velocidadY      = 0.0f;
    enSuelo         = false;
    corriendo       = false;

    stamina       = STAMINA_MAX;
    agotado       = false;
    tiempoAgotado = 0.0f;

    cooldownAtaque   = 0.0f;
    atacando         = false;
    tiempoAnimAtaque = 0.0f;

    float vertices[] = {
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
    unsigned int indices[] = {
         0, 1, 2,  2, 3, 0,
         4, 5, 6,  6, 7, 4,
         8, 9,10, 10,11, 8,
        12,13,14, 14,15,12,
        16,17,18, 18,19,16,
        20,21,22, 22,23,20,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Jugador::~Jugador() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Jugador::actualizarAtaque(float delta) {
    if (cooldownAtaque > 0.0f)   cooldownAtaque   -= delta;
    if (tiempoAnimAtaque > 0.0f) tiempoAnimAtaque -= delta;
    atacando = false;
}

bool Jugador::intentarAtacar() {
    if (cooldownAtaque > 0.0f) return false;
    cooldownAtaque   = COOLDOWN_ATAQUE;
    tiempoAnimAtaque = DURACION_ANIM;
    atacando         = true;
    return true;
}

float Jugador::usarItemSeleccionado() {
    int slot = inventario.getSlotSeleccionado();
    if (slot < 0) return 0.0f;
    Item usado = inventario.usarItem(slot);
    if (!usado.esValido()) return 0.0f;
    return DatosItem::getCuracion(usado.tipo);
}

float Jugador::getVelocidadEfectiva() const {
    // Penalización por peso: 0-50% = sin penalización, 50-100% = hasta -40% vel
    float pct = inventario.getPesoPorcentaje();
    if (pct <= 0.5f) return 1.0f;
    return 1.0f - ((pct - 0.5f) * 0.8f);  // máx -40% a peso lleno
}

void Jugador::actualizarStamina(float delta, bool intentaCorrer) {
    if (agotado) {
        tiempoAgotado += delta;
        stamina += (STAMINA_RECARGA * 0.5f) * delta;
        stamina  = std::min(stamina, STAMINA_MAX);
        if (tiempoAgotado >= DURACION_AGOTAMIENTO && stamina >= STAMINA_UMBRAL_RECOVERY) {
            agotado = false; tiempoAgotado = 0.0f;
        }
        corriendo = false;
    } else if (intentaCorrer && stamina > 0.0f) {
        stamina  -= STAMINA_CONSUMO * delta;
        corriendo = true;
        if (stamina <= 0.0f) {
            stamina = 0.0f; agotado = true; tiempoAgotado = 0.0f; corriendo = false;
        }
    } else {
        stamina  += STAMINA_RECARGA * delta;
        stamina   = std::min(stamina, STAMINA_MAX);
        corriendo = false;
    }
}

void Jugador::mover(glm::vec3 direccion, float delta, bool correr) {
    if (glm::length(direccion) > 0.0f) {
        direccion = glm::normalize(direccion);
        float vel;
        if (agotado)   vel = VELOCIDAD_AGOTADO;
        else if (corriendo) vel = velocidadCorrer;
        else           vel = velocidad;

        // Aplicar penalización de peso
        vel *= getVelocidadEfectiva();

        posicion += direccion * vel * delta;
        rotacion  = glm::degrees(atan2(direccion.x, direccion.z));
    }
}

void Jugador::saltar() {
    if (enSuelo) { velocidadY = FUERZA_SALTO; enSuelo = false; }
}

void Jugador::actualizarFisica(float delta, Mundo& mundo) {
    velocidadY -= GRAVEDAD * delta;
    posicion.y += velocidadY * delta;
    float alturaTerreno = mundo.obtenerAltura(posicion.x, posicion.z) + ALTURA_SOBRE;
    if (posicion.y <= alturaTerreno) {
        posicion.y = alturaTerreno; velocidadY = 0.0f; enSuelo = true;
    } else if (posicion.y <= alturaTerreno + HISTERESIS_SUELO) {
        enSuelo = true;
    } else {
        enSuelo = false;
    }
}

void Jugador::dibujar() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}