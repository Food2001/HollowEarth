#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Zombie;
class Inventario;

class Hud {
public:
    Hud(int anchoVentana, int altoVentana);
    ~Hud();

    void dibujar(float saludPct, float staminaPct, bool agotado, float pesoPct);
    void dibujarBarrasZombies(const std::vector<Zombie>& zombies,
                              const glm::mat4& view, const glm::mat4& projection);
    void dibujarInventario(const Inventario& inv);

private:
    int anchoVentana;
    int altoVentana;

    unsigned int VAO, VBO;
    unsigned int shaderID;

    unsigned int compilarShader(const char* vertSrc, const char* fragSrc);
    void dibujarRect(float x, float y, float ancho, float alto, glm::vec4 color);
    bool proyectar(glm::vec3 pos3D, const glm::mat4& view, const glm::mat4& proj,
                   float& screenX, float& screenY);
};