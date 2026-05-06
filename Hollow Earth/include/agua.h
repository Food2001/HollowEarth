#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader;
class Textura;

class Agua {
public:
    Agua();   // no inicializa OpenGL
    ~Agua();

    // Llamar una vez después de que OpenGL esté listo
    void inicializar();

    void dibujar(Shader& shader, int cx, int cz, float tamanoChunk, Textura* textura = nullptr);

private:
    unsigned int VAO, VBO, EBO;
    bool inicializado = false;
};