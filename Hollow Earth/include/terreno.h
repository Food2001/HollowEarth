#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <FastNoiseLite.h>
#include <vector>
#include <memory>

class Textura;
class Shader;

struct Arbol {
    glm::vec3 posicion;
    float     alturaTronco;
    float     tamanioCopa;
};

class Terreno {
public:
    unsigned int VAO, VBO, EBO;
    int cantidadIndices;
    int chunkX, chunkZ;

    unsigned int arbolVAO, arbolVBO, arbolEBO;
    // === INSTANCIADO ===
    unsigned int arbolInstanceVBO;          // VBO para matrices de instancias
    std::vector<glm::mat4> matricesTroncos; // Cache de matrices de troncos
    std::vector<glm::mat4> matricesCopas;   // Cache de matrices de copas

    const int   RESOLUCION  = 64;
    const float TAMANO      = 50.0f;
    const int   MAX_ARBOLES = 12;

    Terreno(int cx, int cz, FastNoiseLite& ruido);
    ~Terreno();

    void  dibujar(Textura* texturaDifusa = nullptr);
    void  dibujarArboles(Shader& shader);
    float obtenerAltura(float x, float z);

    const std::vector<Arbol>& getArboles() const { return arboles; }

private:
    FastNoiseLite*     ruido;
    std::vector<Arbol> arboles;

    void generarArboles();
    void inicializarCuboArbol();

    float obtenerMezclaTextura(float altura, float& alpha1, float& alpha2, int& tex1, int& tex2);
};