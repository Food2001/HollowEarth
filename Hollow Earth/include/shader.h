#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int id;

    Shader(const char* vertexCodigo, const char* fragmentCodigo);
    ~Shader();

    void usar();
    void setMat4(const std::string& nombre, const float* valor);
    void setVec3(const std::string& nombre, const glm::vec3& valor);
    void setVec2(const std::string& nombre, const glm::vec2& valor);
    void setFloat(const std::string& nombre, float valor);
    void setBool(const std::string& nombre, bool valor);
    void setInt(const std::string& nombre, int valor);
};