#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// ================================================
// Skybox — cubo gigante con cubemap de 6 texturas
// Se renderiza primero, sin escribir al depth buffer
// ================================================
class Skybox {
public:
    Skybox(const std::string& carpeta);
    ~Skybox();

    // view sin traslación, projection normal
    void dibujar(const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO, VBO;
    unsigned int cubemapID;
    unsigned int shaderID;

    unsigned int cargarCubemap(const std::string& carpeta);
    unsigned int compilarShader(const char* vert, const char* frag);
};