#pragma once
#include <vector>
#include <string>
#include <deque>
#include <glm/glm.hpp>

// ================================================
// Gestor de Actualizaciones — H.E.
// Controla cuándo se actualiza cada sistema
// según distancia al jugador y escalonado por grupos
//
// Zonas de distancia:
//   CERCANA  (0-30u)  → cada frame
//   MEDIA    (30-80u) → cada 3 frames
//   LEJANA   (80u+)   → cada 8 frames
// ================================================

// Stats que el gestor reporta a la consola
struct StatsGestor {
    int zombiesTotales    = 0;
    int zombiesActivos    = 0;   // actualizados este frame
    int zombiesDormidos   = 0;   // saltados este frame
    int chunksActivos     = 0;
    int drawCalls         = 0;
    int particulasActivas = 0;
};

class Gestor {
public:
    // Frecuencias de actualización por zona
    static constexpr int FREQ_CERCANA = 1;   // cada frame
    static constexpr int FREQ_MEDIA   = 3;   // cada 3 frames
    static constexpr int FREQ_LEJANA  = 8;   // cada 8 frames

    // Radios de zona
    static constexpr float RADIO_CERCANO = 30.0f;
    static constexpr float RADIO_MEDIO   = 80.0f;

    // Tamaño del batch para zombies
    static constexpr int TAMANO_BATCH = 50;

    Gestor();

    // Llamar al inicio de cada frame
    void iniciarFrame();

    // ¿Debe actualizarse esta entidad este frame?
    // posEntidad = posición de la entidad
    // posJugador = posición del jugador
    // indice     = índice de la entidad en su vector (para escalonado)
    bool debeActualizar(glm::vec3 posEntidad, glm::vec3 posJugador, int indice) const;

    // Versión simplificada sin posición (para sistemas globales)
    bool debeActualizarGlobal(int indice, int frecuencia) const;

    // Stats para la consola
    StatsGestor& getStats() { return stats; }
    const StatsGestor& getStats() const { return stats; }
    void resetStats() { stats = StatsGestor{}; }

    int getFrameActual() const { return frameActual; }

    // ── Historial de FPS ──────────────────────────────────
    void registrarFPS(float fps);
    float getFPSPromedio() const;
    float getFPSMinimo()   const;
    float getFPSMaximo()   const;
    const std::deque<float>& getHistorial() const { return historialFPS; }

    // ── Detección de tirones ──────────────────────────────
    // Un tirón es un frame donde delta > umbralTiron
    void registrarDelta(float delta, int zombies, int chunks);

    struct Tiron {
        float delta;
        int   zombies;
        int   chunks;
        int   frame;
    };
    const std::vector<Tiron>& getTirones() const { return tirones; }
private:
    std::deque<float> tiemposZombies;
    static constexpr int MAX_TIEMPOS_ZOMBIES = 20;

public:
    void registrarTiempoZombies(float tiempo);
    float getTiempoPromedioZombies() const;

    private:
    int frameActual;

    StatsGestor stats;

    // Historial de FPS — últimos 60 valores (60 segundos)
    std::deque<float> historialFPS;
    static constexpr int MAX_HISTORIAL = 60;

    // Tirones detectados
    std::vector<Tiron> tirones;
    static constexpr float UMBRAL_TIRON = 0.033f;  // < 30 FPS
    static constexpr int   MAX_TIRONES  = 20;       // guardar últimos 20
};