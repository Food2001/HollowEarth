#include "shader.h"
#include <iostream>

Shader::Shader(const char* vertexCodigo, const char* fragmentCodigo) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCodigo, nullptr);
    glCompileShader(vertexShader);

    int exito; char log[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &exito);
    if (!exito) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, log);
        std::cout << "Error vertex shader: " << log << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCodigo, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &exito);
    if (!exito) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, log);
        std::cout << "Error fragment shader: " << log << std::endl;
    }

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &exito);
    if (!exito) {
        glGetProgramInfoLog(id, 512, nullptr, log);
        std::cout << "Error link shader: " << log << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() { glDeleteProgram(id); }
void Shader::usar()  { glUseProgram(id); }

void Shader::setMat4(const std::string& nombre, const float* valor) {
    glUniformMatrix4fv(glGetUniformLocation(id, nombre.c_str()), 1, GL_FALSE, valor);
}
void Shader::setVec3(const std::string& nombre, const glm::vec3& valor) {
    glUniform3fv(glGetUniformLocation(id, nombre.c_str()), 1, &valor[0]);
}
void Shader::setVec2(const std::string& nombre, const glm::vec2& valor) {
    glUniform2fv(glGetUniformLocation(id, nombre.c_str()), 1, &valor[0]);
}
void Shader::setFloat(const std::string& nombre, float valor) {
    glUniform1f(glGetUniformLocation(id, nombre.c_str()), valor);
}
void Shader::setBool(const std::string& nombre, bool valor) {
    glUniform1i(glGetUniformLocation(id, nombre.c_str()), valor ? 1 : 0);
}
void Shader::setInt(const std::string& nombre, int valor) {
    glUniform1i(glGetUniformLocation(id, nombre.c_str()), valor);
}