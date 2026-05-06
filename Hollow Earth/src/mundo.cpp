#include "mundo.h"
#include "textura.h"
#include "zombie.h"
#include <cmath>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>

Mundo::Mundo() {
    ruido.SetSeed(static_cast<int>(time(NULL)));
    ruido.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    ruido.SetFractalType(FastNoiseLite::FractalType_FBm);
    ruido.SetFrequency(0.005f);
    ruido.SetFractalOctaves(4);
}

Mundo::~Mundo() {}

void Mundo::inicializar() {
    agua.inicializar();
}

std::string Mundo::clave(int cx, int cz) {
    return std::to_string(cx) + "," + std::to_string(cz);
}

void Mundo::cargarChunk(int cx, int cz) {
    std::string k = clave(cx, cz);
    if (chunks.find(k) == chunks.end()) {
        chunks[k] = std::make_unique<Terreno>(cx, cz, ruido);
        chunksNuevos.push_back({cx, cz});  // marcar como nuevo para spawn
    }
}

void Mundo::descargarLejanos(int centroX, int centroZ) {
    auto it = chunks.begin();
    while (it != chunks.end()) {
        Terreno* t = it->second.get();
        int dx = abs(t->chunkX - centroX);
        int dz = abs(t->chunkZ - centroZ);
        if (dx > RENDER_DIST || dz > RENDER_DIST) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void Mundo::actualizar(float jugadorX, float jugadorZ) {
    chunksNuevos.clear();
    int centroX = (int)floor(jugadorX / TAMANO_CHUNK);
    int centroZ = (int)floor(jugadorZ / TAMANO_CHUNK);
    for (int z = -RENDER_DIST; z <= RENDER_DIST; z++)
        for (int x = -RENDER_DIST; x <= RENDER_DIST; x++)
            cargarChunk(centroX + x, centroZ + z);
    // Forzar límite de 81 chunks (9x9)
    while (chunks.size() > 81) {
        descargarLejanos(centroX, centroZ);
    }
}

float Mundo::obtenerAltura(float wx, float wz) {
    int cx = static_cast<int>(floor(wx / TAMANO_CHUNK));
    int cz = static_cast<int>(floor(wz / TAMANO_CHUNK));
    std::string k = clave(cx, cz);
    auto it = chunks.find(k);
    if (it != chunks.end())
        return it->second->obtenerAltura(wx, wz);
    float n = ruido.GetNoise(wx, wz);
    return (n >= 0.0f) ? pow(n, 1.8f) * 22.0f : n * 2.5f;
}

void Mundo::generarZombiesChunk(int cx, int cz, std::vector<Zombie>& zombies) {
    float offsetX = cx * TAMANO_CHUNK;
    float offsetZ = cz * TAMANO_CHUNK;

    // Semilla determinista por chunk — mismos zombies siempre
    unsigned int semilla = (unsigned int)(cx * 48397211 ^ cz * 83492791);
    srand(semilla);

    int cantidad = MIN_Z_CHUNK + rand() % (MAX_Z_CHUNK - MIN_Z_CHUNK + 1);

    for (int i = 0; i < cantidad; i++) {
        float px = offsetX + 2.0f + (rand() / (float)RAND_MAX) * (TAMANO_CHUNK - 4.0f);
        float pz = offsetZ + 2.0f + (rand() / (float)RAND_MAX) * (TAMANO_CHUNK - 4.0f);
        float py = std::max(3.0f, obtenerAltura(px, pz) + 3.0f);
        zombies.emplace_back(glm::vec3(px, py, pz));
    }
}

// === NUEVO: Versión con Frustum Culling ===
void Mundo::dibujar(const glm::mat4& viewProj, Textura* textura) {
    for (auto const& [key, chunk] : chunks) {
        if (estaEnFrustum(viewProj, chunk->chunkX, chunk->chunkZ)) {
            chunk->dibujar(textura);
        }
    }
}

// === NUEVO: Versión con Frustum Culling ===
void Mundo::dibujarArboles(Shader& shader, const glm::mat4& viewProj) {
    for (auto const& [key, chunk] : chunks) {
        if (estaEnFrustum(viewProj, chunk->chunkX, chunk->chunkZ)) {
            chunk->dibujarArboles(shader);
        }
    }
}

bool Mundo::chunkTieneAgua(int cx, int cz) {
    float offsetX = cx * TAMANO_CHUNK;
    float offsetZ = cz * TAMANO_CHUNK;
    float paso    = TAMANO_CHUNK / 4.0f;
    for (int z = 0; z <= 4; z++) {
        for (int x = 0; x <= 4; x++) {
            float wx = offsetX + x * paso;
            float wz = offsetZ + z * paso;
            float n  = ruido.GetNoise(wx, wz);
            float h  = (n >= 0.0f) ? pow(n, 1.8f) * 22.0f : n * 2.5f;
            if (h < 0.0f) return true;
        }
    }
    return false;
}

// === NUEVO: Versión con Frustum Culling ===
void Mundo::dibujarAgua(Shader& shader, const glm::mat4& viewProj, Textura* texturaAgua) {
    for (auto const& [key, chunk] : chunks) {
        if (estaEnFrustum(viewProj, chunk->chunkX, chunk->chunkZ) && chunkTieneAgua(chunk->chunkX, chunk->chunkZ)) {
            agua.dibujar(shader, chunk->chunkX, chunk->chunkZ, TAMANO_CHUNK, texturaAgua);
        }
    }
}

// === NUEVO: Frustum Culling Básico ===
bool Mundo::estaEnFrustum(const glm::mat4& viewProj, int cx, int cz) const {
    // Centro del chunk en coordenadas del mundo
    float centroX = (cx + 0.5f) * TAMANO_CHUNK;
    float centroZ = (cz + 0.5f) * TAMANO_CHUNK;
    // Extensión del AABB (caja alineada a ejes) del chunk
    float radio = TAMANO_CHUNK * 0.5f + 5.0f; // margen extra para árboles altos

    // Comprobar las 6 caras del frustum
    // Usamos el plano de clipping en espacio de clip (viewProj * vec4)
    glm::vec4 esquinas[8] = {
        viewProj * glm::vec4(centroX - radio, 0.0f,        centroZ - radio, 1.0f),
        viewProj * glm::vec4(centroX + radio, 0.0f,        centroZ - radio, 1.0f),
        viewProj * glm::vec4(centroX - radio, 0.0f,        centroZ + radio, 1.0f),
        viewProj * glm::vec4(centroX + radio, 0.0f,        centroZ + radio, 1.0f),
        viewProj * glm::vec4(centroX - radio, TAMANO_CHUNK, centroZ - radio, 1.0f),
        viewProj * glm::vec4(centroX + radio, TAMANO_CHUNK, centroZ - radio, 1.0f),
        viewProj * glm::vec4(centroX - radio, TAMANO_CHUNK, centroZ + radio, 1.0f),
        viewProj * glm::vec4(centroX + radio, TAMANO_CHUNK, centroZ + radio, 1.0f)
    };

    // Si TODAS las esquinas están fuera de algún plano (x < -w, x > w, etc.), no se ve.
    // Simplificamos: comprobar si algún punto está dentro del volumen de clip.
    // Método rápido: si todos los puntos tienen x > w, o todos x < -w, etc., entonces está fuera.
    bool todoFueraIzq = true, todoFueraDer = true;
    bool todoFueraAbj = true, todoFueraArr = true;
    bool todoFueraCer = true, todoFueraLej = true;

    for (int i = 0; i < 8; i++) {
        glm::vec4 v = esquinas[i];
        if (v.w > 0) { // Evitar división por cero y puntos detrás de la cámara
            float x = v.x / v.w;
            float y = v.y / v.w;
            float z = v.z / v.w;

            if (x > -1.0f) todoFueraIzq = false;
            if (x <  1.0f) todoFueraDer = false;
            if (y > -1.0f) todoFueraAbj = false;
            if (y <  1.0f) todoFueraArr = false;
            if (z > -1.0f) todoFueraCer = false;
            if (z <  1.0f) todoFueraLej = false;
        } else {
            // Un punto detrás de la cámara no significa que todo el chunk esté fuera,
            // así que asumimos que podría ser visible.
            return true;
        }
    }

    return !(todoFueraIzq || todoFueraDer || todoFueraAbj || todoFueraArr || todoFueraCer || todoFueraLej);
}