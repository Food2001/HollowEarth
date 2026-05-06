#include "terreno.h"
#include "textura.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

// Normal de triángulo
glm::vec3 calcularNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 u = b - a;
    glm::vec3 v = c - a;
    return glm::normalize(glm::cross(u, v));
}

// Altura del terreno
static float calcularAltura(FastNoiseLite* ruido, float wx, float wz) {
    float n = ruido->GetNoise(wx, wz);
    if (n >= 0.0f)
        return pow(n, 1.8f) * 22.0f;
    else
        return n * 2.5f;
}

// Cubo unitario compartido para los árboles
static float CUBO_VERTICES[] = {
    // Front
    -0.5f,-0.5f, 0.5f,  0,0,1,  0,0,
     0.5f,-0.5f, 0.5f,  0,0,1,  1,0,
     0.5f, 0.5f, 0.5f,  0,0,1,  1,1,
    -0.5f, 0.5f, 0.5f,  0,0,1,  0,1,
    // Back
    -0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
     0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
     0.5f, 0.5f,-0.5f,  0,0,-1, 1,1,
    -0.5f, 0.5f,-0.5f,  0,0,-1, 0,1,
    // Left
    -0.5f,-0.5f,-0.5f, -1,0,0,  0,0,
    -0.5f,-0.5f, 0.5f, -1,0,0,  1,0,
    -0.5f, 0.5f, 0.5f, -1,0,0,  1,1,
    -0.5f, 0.5f,-0.5f, -1,0,0,  0,1,
    // Right
     0.5f,-0.5f,-0.5f,  1,0,0,  0,0,
     0.5f,-0.5f, 0.5f,  1,0,0,  1,0,
     0.5f, 0.5f, 0.5f,  1,0,0,  1,1,
     0.5f, 0.5f,-0.5f,  1,0,0,  0,1,
    // Top
    -0.5f, 0.5f,-0.5f,  0,1,0,  0,0,
     0.5f, 0.5f,-0.5f,  0,1,0,  1,0,
     0.5f, 0.5f, 0.5f,  0,1,0,  1,1,
    -0.5f, 0.5f, 0.5f,  0,1,0,  0,1,
    // Bottom
    -0.5f,-0.5f,-0.5f,  0,-1,0, 0,0,
     0.5f,-0.5f,-0.5f,  0,-1,0, 1,0,
     0.5f,-0.5f, 0.5f,  0,-1,0, 1,1,
    -0.5f,-0.5f, 0.5f,  0,-1,0, 0,1,
};
static unsigned int CUBO_INDICES[] = {
     0, 1, 2,  2, 3, 0,
     4, 5, 6,  6, 7, 4,
     8, 9,10, 10,11, 8,
    12,13,14, 14,15,12,
    16,17,18, 18,19,16,
    20,21,22, 22,23,20,
};

// Constructor
Terreno::Terreno(int cx, int cz, FastNoiseLite& r) {
    chunkX = cx;
    chunkZ = cz;
    ruido  = &r;

    // Malla del terreno
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    float paso    = TAMANO / (float)(RESOLUCION - 1);
    float offsetX = cx * TAMANO;
    float offsetZ = cz * TAMANO;

    struct Punto { glm::vec3 pos; float altura; };
    std::vector<Punto> puntos(RESOLUCION * RESOLUCION);

    for (int z = 0; z < RESOLUCION; z++) {
        for (int x = 0; x < RESOLUCION; x++) {
            int   i  = x + z * RESOLUCION;
            float wx = offsetX + (x * paso);
            float wz = offsetZ + (z * paso);
            float h  = calcularAltura(ruido, wx, wz);
            puntos[i].pos    = glm::vec3(wx, h, wz);
            puntos[i].altura = h;
        }
    }

    std::vector<glm::vec3> normales(puntos.size(), glm::vec3(0.0f));

    for (int z = 0; z < RESOLUCION - 1; z++) {
        for (int x = 0; x < RESOLUCION - 1; x++) {
            int i0 = x     +  z    * RESOLUCION;
            int i1 = (x+1) +  z    * RESOLUCION;
            int i2 = x     + (z+1) * RESOLUCION;
            int i3 = (x+1) + (z+1) * RESOLUCION;

            glm::vec3 n1 = calcularNormal(puntos[i0].pos, puntos[i2].pos, puntos[i1].pos);
            normales[i0] += n1; normales[i2] += n1; normales[i1] += n1;
            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);

            glm::vec3 n2 = calcularNormal(puntos[i1].pos, puntos[i2].pos, puntos[i3].pos);
            normales[i1] += n2; normales[i2] += n2; normales[i3] += n2;
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }

    for (auto& n : normales) n = glm::normalize(n);

    for (size_t i = 0; i < puntos.size(); i++) {
        float u = puntos[i].pos.x / 8.0f;
        float v = puntos[i].pos.z / 8.0f;
        vertices.push_back(puntos[i].pos.x);
        vertices.push_back(puntos[i].pos.y);
        vertices.push_back(puntos[i].pos.z);
        vertices.push_back(normales[i].x);
        vertices.push_back(normales[i].y);
        vertices.push_back(normales[i].z);
        vertices.push_back(u);
        vertices.push_back(v);
    }

    cantidadIndices = (int)indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Cubo para árboles
    inicializarCuboArbol();

    // Árboles de este chunk
    generarArboles();
}

void Terreno::inicializarCuboArbol() {
    glGenVertexArrays(1, &arbolVAO);
    glGenBuffers(1, &arbolVBO);
    glGenBuffers(1, &arbolEBO);
    // === INSTANCIADO === VBO para las matrices de instancia
    glGenBuffers(1, &arbolInstanceVBO);

    glBindVertexArray(arbolVAO);
    glBindBuffer(GL_ARRAY_BUFFER, arbolVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBO_VERTICES), CUBO_VERTICES, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arbolEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CUBO_INDICES), CUBO_INDICES, GL_STATIC_DRAW);

    // Atributos estándar del cubo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // === INSTANCIADO === Configurar atributos de instancia (location 3,4,5,6 para mat4)
    glBindBuffer(GL_ARRAY_BUFFER, arbolInstanceVBO);
    size_t vec4Size = sizeof(glm::vec4);
    // Columna 0
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glVertexAttribDivisor(3, 1);
    // Columna 1
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glVertexAttribDivisor(4, 1);
    // Columna 2
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glVertexAttribDivisor(5, 1);
    // Columna 3
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void Terreno::generarArboles() {
    float offsetX = chunkX * TAMANO;
    float offsetZ = chunkZ * TAMANO;

    // Cachear alturas del terreno
    std::vector<float> alturasCache(RESOLUCION * RESOLUCION);
    float paso = TAMANO / (float)(RESOLUCION - 1);
    for (int z = 0; z < RESOLUCION; z++) {
        for (int x = 0; x < RESOLUCION; x++) {
            float wx = offsetX + x * paso;
            float wz = offsetZ + z * paso;
            alturasCache[x + z * RESOLUCION] = calcularAltura(ruido, wx, wz);
        }
    }

    // Semilla determinista por chunk — mismos árboles siempre
    unsigned int semilla = (unsigned int)(chunkX * 73856093 ^ chunkZ * 19349663);
    srand(semilla);

    int cantidad = rand() % (MAX_ARBOLES + 1);
    arboles.clear();
    for (int i = 0; i < cantidad; i++) {
        float rx = offsetX + 2.0f + (rand() / (float)RAND_MAX) * (TAMANO - 4.0f);
        float rz = offsetZ + 2.0f + (rand() / (float)RAND_MAX) * (TAMANO - 4.0f);
        // Usar alturas cacheadas (interpolar si es necesario)
        int ix = (int)((rx - offsetX) / paso);
        int iz = (int)((rz - offsetZ) / paso);
        float h = alturasCache[ix + iz * RESOLUCION];

        if (h < 0.5f || h > 12.0f) continue;

        Arbol a;
        a.posicion     = glm::vec3(rx, h, rz);
        a.alturaTronco = 2.0f + (rand() / (float)RAND_MAX) * 1.5f;  // 2 — 3.5
        a.tamanioCopa  = 2.5f + (rand() / (float)RAND_MAX) * 1.5f;  // 2.5 — 4
        arboles.push_back(a);
    }

    // === INSTANCIADO === Generar matrices de instancia para troncos y copas
    matricesTroncos.clear();
    matricesCopas.clear();
    for (const Arbol& a : arboles) {
        // Tronco
        glm::mat4 mTronco = glm::translate(glm::mat4(1.0f),
            glm::vec3(a.posicion.x, a.posicion.y + a.alturaTronco * 0.5f, a.posicion.z));
        mTronco = glm::scale(mTronco, glm::vec3(0.35f, a.alturaTronco, 0.35f));
        matricesTroncos.push_back(mTronco);

        // Copa
        glm::mat4 mCopa = glm::translate(glm::mat4(1.0f),
            glm::vec3(a.posicion.x, a.posicion.y + a.alturaTronco + a.tamanioCopa * 0.5f, a.posicion.z));
        mCopa = glm::scale(mCopa, glm::vec3(a.tamanioCopa, a.tamanioCopa * 0.8f, a.tamanioCopa));
        matricesCopas.push_back(mCopa);
    }
}

// Destructor
Terreno::~Terreno() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &arbolVAO);
    glDeleteBuffers(1, &arbolVBO);
    glDeleteBuffers(1, &arbolEBO);
    glDeleteBuffers(1, &arbolInstanceVBO); // === INSTANCIADO ===
}

// Dibujar terreno
void Terreno::dibujar(Textura* texturaDifusa) {
    if (texturaDifusa) texturaDifusa->usar(0);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cantidadIndices, GL_UNSIGNED_INT, 0);
}

// Dibujar árboles — AHORA CON INSTANCIADO
void Terreno::dibujarArboles(Shader& shader) {
    if (arboles.empty()) return;

    glBindVertexArray(arbolVAO);
    shader.setBool("usarTextura", false);

    // === INSTANCIADO === Subir datos de troncos y dibujar
    glBindBuffer(GL_ARRAY_BUFFER, arbolInstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, matricesTroncos.size() * sizeof(glm::mat4), matricesTroncos.data(), GL_STATIC_DRAW);
    shader.setVec3("colorSolido", glm::vec3(0.45f, 0.28f, 0.12f));  // marrón
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, (GLsizei)matricesTroncos.size());

    // === INSTANCIADO === Subir datos de copas y dibujar
    glBufferData(GL_ARRAY_BUFFER, matricesCopas.size() * sizeof(glm::mat4), matricesCopas.data(), GL_STATIC_DRAW);
    shader.setVec3("colorSolido", glm::vec3(0.15f, 0.55f, 0.15f));  // verde
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, (GLsizei)matricesCopas.size());

    glBindVertexArray(0);
}

float Terreno::obtenerAltura(float x, float z) {
    return calcularAltura(ruido, x, z);
}