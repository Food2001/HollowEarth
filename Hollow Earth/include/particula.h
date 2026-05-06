#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;

struct Particula {
    glm::vec3 posicion;
    glm::vec3 velocidad;
    float     vida;       // 0.0 = muerta
    float     vidaMax;
    float     tamanio;
    glm::vec3 color;
};

class SistemaParticulas {
public:
    SistemaParticulas();
    ~SistemaParticulas();

    // Emitir explosión de partículas en una posición
    void emitirMuerteZombie(glm::vec3 posicion);

    void actualizar(float delta);
    void dibujar(Shader& shader);

    bool hayParticulas() const { return !particulas.empty(); }

private:
    std::vector<Particula> particulas;
    unsigned int VAO, VBO, EBO;
};