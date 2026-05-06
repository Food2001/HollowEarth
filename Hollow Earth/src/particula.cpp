#include "particula.h"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <cmath>
#include <algorithm>

// Cubo unitario para las partículas
static float P_VERTICES[] = {
    -0.5f,-0.5f, 0.5f,  0,0,1,  0,0,
     0.5f,-0.5f, 0.5f,  0,0,1,  1,0,
     0.5f, 0.5f, 0.5f,  0,0,1,  1,1,
    -0.5f, 0.5f, 0.5f,  0,0,1,  0,1,
    -0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
     0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
     0.5f, 0.5f,-0.5f,  0,0,-1, 1,1,
    -0.5f, 0.5f,-0.5f,  0,0,-1, 0,1,
    -0.5f,-0.5f,-0.5f, -1,0,0,  0,0,
    -0.5f,-0.5f, 0.5f, -1,0,0,  1,0,
    -0.5f, 0.5f, 0.5f, -1,0,0,  1,1,
    -0.5f, 0.5f,-0.5f, -1,0,0,  0,1,
     0.5f,-0.5f,-0.5f,  1,0,0,  0,0,
     0.5f,-0.5f, 0.5f,  1,0,0,  1,0,
     0.5f, 0.5f, 0.5f,  1,0,0,  1,1,
     0.5f, 0.5f,-0.5f,  1,0,0,  0,1,
    -0.5f, 0.5f,-0.5f,  0,1,0,  0,0,
     0.5f, 0.5f,-0.5f,  0,1,0,  1,0,
     0.5f, 0.5f, 0.5f,  0,1,0,  1,1,
    -0.5f, 0.5f, 0.5f,  0,1,0,  0,1,
    -0.5f,-0.5f,-0.5f,  0,-1,0, 0,0,
     0.5f,-0.5f,-0.5f,  0,-1,0, 1,0,
     0.5f,-0.5f, 0.5f,  0,-1,0, 1,1,
    -0.5f,-0.5f, 0.5f,  0,-1,0, 0,1,
};
static unsigned int P_INDICES[] = {
     0, 1, 2,  2, 3, 0,
     4, 5, 6,  6, 7, 4,
     8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20,
};

SistemaParticulas::SistemaParticulas() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(P_VERTICES), P_VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(P_INDICES), P_INDICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

SistemaParticulas::~SistemaParticulas() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void SistemaParticulas::emitirMuerteZombie(glm::vec3 posicion) {
    const int CANTIDAD = 12;

    // Colores de sangre/carne — rojo oscuro, marrón, gris
    glm::vec3 colores[] = {
        glm::vec3(0.7f, 0.05f, 0.05f),
        glm::vec3(0.5f, 0.08f, 0.08f),
        glm::vec3(0.4f, 0.15f, 0.10f),
        glm::vec3(0.3f, 0.10f, 0.10f),
    };

    for (int i = 0; i < CANTIDAD; i++) {
        Particula p;
        p.posicion = posicion + glm::vec3(0.0f, 0.5f, 0.0f);

        // Velocidad aleatoria en todas direcciones, más hacia arriba
        float angH  = (rand() / (float)RAND_MAX) * 360.0f;
        float radH  = glm::radians(angH);
        float velH  = 2.0f + (rand() / (float)RAND_MAX) * 4.0f;
        float velV  = 2.0f + (rand() / (float)RAND_MAX) * 5.0f;

        p.velocidad = glm::vec3(
            sin(radH) * velH,
            velV,
            cos(radH) * velH
        );

        p.vidaMax  = 0.4f + (rand() / (float)RAND_MAX) * 0.4f;
        p.vida     = p.vidaMax;
        p.tamanio  = 0.08f + (rand() / (float)RAND_MAX) * 0.15f;
        p.color    = colores[rand() % 4];

        particulas.push_back(p);
    }
}

void SistemaParticulas::actualizar(float delta) {
    const float GRAVEDAD = 9.8f;

    for (auto& p : particulas) {
        if (p.vida <= 0.0f) continue;
        p.velocidad.y -= GRAVEDAD * delta;
        p.posicion    += p.velocidad * delta;
        p.vida        -= delta;
    }

    // Eliminar partículas muertas
    if (!particulas.empty()) {
        particulas.erase(
            std::remove_if(particulas.begin(), particulas.end(),
                [](const Particula& p){ return p.vida <= 0.0f; }),
            particulas.end()
        );
    }
}

void SistemaParticulas::dibujar(Shader& shader) {
    if (particulas.empty()) return;

    glBindVertexArray(VAO);
    shader.setBool("usarTextura", false);
    shader.setFloat("alpha", 1.0f);

    for (const Particula& p : particulas) {
        // Fade out al final de la vida
        float alpha = p.vida / p.vidaMax;

        glm::mat4 m = glm::mat4(1.0f);
        m = glm::translate(m, p.posicion);
        m = glm::scale(m, glm::vec3(p.tamanio));

        shader.setMat4("model", glm::value_ptr(m));
        shader.setVec3("colorSolido", p.color * alpha);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}