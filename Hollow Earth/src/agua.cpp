#include "agua.h"
#include "shader.h"
#include "textura.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>

static float PLANO_VERTICES[] = {
    -0.5f, 0.0f, -0.5f,  0,1,0,  0,0,
     0.5f, 0.0f, -0.5f,  0,1,0,  1,0,
     0.5f, 0.0f,  0.5f,  0,1,0,  1,1,
    -0.5f, 0.0f,  0.5f,  0,1,0,  0,1,
};
static unsigned int PLANO_INDICES[] = { 0,1,2, 2,3,0 };

Agua::Agua() : VAO(0), VBO(0), EBO(0), inicializado(false) {
    // NO llamar a OpenGL aquí — puede ejecutarse antes de GLAD
}

void Agua::inicializar() {
    if (inicializado) return;
    inicializado = true;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PLANO_VERTICES), PLANO_VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PLANO_INDICES), PLANO_INDICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

Agua::~Agua() {
    if (inicializado) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Agua::dibujar(Shader& shader, int cx, int cz, float tamanoChunk, Textura* textura) {
    if (!inicializado) return;

    float centroX = (cx + 0.5f) * tamanoChunk;
    float centroZ = (cz + 0.5f) * tamanoChunk;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(centroX, 0.0f, centroZ));
    model = glm::scale(model, glm::vec3(tamanoChunk, 1.0f, tamanoChunk));

    shader.setMat4("model", glm::value_ptr(model));
    shader.setFloat("alpha", 0.72f);

    if (textura) {
        float t       = (float)SDL_GetTicks() / 1000.0f;
        float offsetU = fmod(t * 0.03f, 1.0f);
        float offsetV = fmod(t * 0.02f, 1.0f);
        shader.setBool("usarTextura",      true);
        shader.setBool("usarMultiTextura", false);
        shader.setInt("texturaDifusa", 0);
        shader.setVec2("uvOffset", glm::vec2(offsetU, offsetV));
        textura->usar(0);
    } else {
        shader.setBool("usarTextura",      false);
        shader.setBool("usarMultiTextura", false);
        shader.setVec3("colorSolido", glm::vec3(0.15f, 0.45f, 0.75f));
        shader.setVec2("uvOffset", glm::vec2(0.0f, 0.0f));
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}