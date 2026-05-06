#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <FastNoiseLite.h>
#include "terreno.h"
#include "shader.h"
#include "agua.h"

class Textura;
class Zombie;

class Mundo {
public:
    const int   RENDER_DIST  = 4;
    const float TAMANO_CHUNK = 50.0f;

    // Zombies por chunk — 3 a 8, semilla determinista
    static constexpr int MIN_Z_CHUNK = 3;
    static constexpr int MAX_Z_CHUNK = 8;

    Mundo();
    ~Mundo();

    void inicializar();

    void  actualizar(float jugadorX, float jugadorZ);
    
    // === NUEVO: ahora reciben la matriz viewProj para frustum culling ===
    void  dibujar(const glm::mat4& viewProj, Textura* textura = nullptr);
    void  dibujarArboles(Shader& shader, const glm::mat4& viewProj);
    void  dibujarAgua(Shader& shader, const glm::mat4& viewProj, Textura* texturaAgua = nullptr);
    
    float obtenerAltura(float x, float z);

    // Spawn de zombies para chunks recién cargados
    void generarZombiesChunk(int cx, int cz, std::vector<Zombie>& zombies);

    // Chunks recién cargados este frame (para spawnear zombies)
    const std::vector<std::pair<int,int>>& getChunksNuevos() const { return chunksNuevos; }
    void limpiarChunksNuevos() { chunksNuevos.clear(); }

    int getCantidadChunks() const { return (int)chunks.size(); }

    // Frustum culling: determina si un chunk es visible
    bool estaEnFrustum(const glm::mat4& viewProj, int cx, int cz) const;

private:
    std::unordered_map<std::string, std::unique_ptr<Terreno>> chunks;
    FastNoiseLite ruido;
    Agua          agua;

    // Chunks que se cargaron en este frame
    std::vector<std::pair<int,int>> chunksNuevos;

    std::string clave(int cx, int cz);
    void cargarChunk(int cx, int cz);
    void descargarLejanos(int centroX, int centroZ);
    bool chunkTieneAgua(int cx, int cz);
};