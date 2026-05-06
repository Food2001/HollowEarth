#include "hud.h"
#include "zombie.h"
#include "inventario.h"
#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

static const char* HUD_VERT = R"(
#version 330 core
layout (location = 0) in vec2 pos;
uniform mat4 proyeccion;
void main() {
    gl_Position = proyeccion * vec4(pos, 0.0, 1.0);
}
)";

static const char* HUD_FRAG = R"(
#version 330 core
out vec4 colorFinal;
uniform vec4 color;
void main() {
    colorFinal = color;
}
)";

unsigned int Hud::compilarShader(const char* vertSrc, const char* fragSrc) {
    int exito; char log[512];
    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vertSrc, nullptr);
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &exito);
    if (!exito) { glGetShaderInfoLog(v, 512, nullptr, log); std::cout << "HUD vert: " << log << std::endl; }
    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fragSrc, nullptr);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &exito);
    if (!exito) { glGetShaderInfoLog(f, 512, nullptr, log); std::cout << "HUD frag: " << log << std::endl; }
    unsigned int p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

Hud::Hud(int anchoVentana, int altoVentana)
    : anchoVentana(anchoVentana), altoVentana(altoVentana) {
    shaderID = compilarShader(HUD_VERT, HUD_FRAG);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Hud::~Hud() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderID);
}

void Hud::dibujarRect(float x, float y, float ancho, float alto, glm::vec4 color) {
    float vertices[12] = {
        x,       y,        x+ancho, y,        x+ancho, y+alto,
        x,       y,        x+ancho, y+alto,   x,       y+alto,
    };
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glm::mat4 proj = glm::ortho(0.0f, (float)anchoVentana, 0.0f, (float)altoVentana, -1.0f, 1.0f);
    glUseProgram(shaderID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proyeccion"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform4fv(glGetUniformLocation(shaderID, "color"), 1, glm::value_ptr(color));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Hud::dibujar(float saludPct, float staminaPct, bool agotado, float pesoPct) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float MARGEN      = 16.0f;
    const float BARRA_ANCHO = 180.0f;
    const float BARRA_ALTO  = 14.0f;
    const float BORDE       = 2.0f;
    const float SEP         = 20.0f;

    // ── Salud ─────────────────────────────────────────────
    float saludY = MARGEN + SEP * 2;
    dibujarRect(MARGEN, saludY, BARRA_ANCHO, BARRA_ALTO, glm::vec4(0.1f,0.1f,0.1f,0.8f));
    glm::vec4 colorSalud = saludPct > 0.6f
        ? glm::vec4(0.2f,0.8f,0.2f,1.0f)
        : saludPct > 0.3f
            ? glm::vec4(0.9f,0.75f,0.1f,1.0f)
            : glm::vec4(0.85f,0.15f,0.15f,1.0f);
    dibujarRect(MARGEN+BORDE, saludY+BORDE, (BARRA_ANCHO-BORDE*2)*saludPct, BARRA_ALTO-BORDE*2, colorSalud);

    // ── Stamina ───────────────────────────────────────────
    float staminaY = MARGEN + SEP;
    dibujarRect(MARGEN, staminaY, BARRA_ANCHO, BARRA_ALTO, glm::vec4(0.1f,0.1f,0.1f,0.8f));
    glm::vec4 colorStamina;
    if (agotado) {
        float t     = (float)SDL_GetTicks() / 1000.0f;
        float pulso = (sin(t * 6.0f) + 1.0f) * 0.5f;
        colorStamina = glm::mix(glm::vec4(0.8f,0.2f,0.0f,1.0f), glm::vec4(0.5f,0.1f,0.0f,1.0f), pulso);
    } else if (staminaPct > 0.5f) {
        colorStamina = glm::vec4(0.2f,0.6f,0.9f,1.0f);
    } else if (staminaPct > 0.2f) {
        colorStamina = glm::vec4(0.9f,0.75f,0.1f,1.0f);
    } else {
        colorStamina = glm::vec4(0.9f,0.4f,0.1f,1.0f);
    }
    dibujarRect(MARGEN+BORDE, staminaY+BORDE, (BARRA_ANCHO-BORDE*2)*staminaPct, BARRA_ALTO-BORDE*2, colorStamina);

    // ── Peso ──────────────────────────────────────────────
    float pesoY = MARGEN;
    dibujarRect(MARGEN, pesoY, BARRA_ANCHO, BARRA_ALTO, glm::vec4(0.1f,0.1f,0.1f,0.8f));
    glm::vec4 colorPeso = pesoPct < 0.5f
        ? glm::vec4(0.7f,0.6f,0.3f,1.0f)   // marrón claro = liviano
        : pesoPct < 0.9f
            ? glm::vec4(0.8f,0.4f,0.1f,1.0f)  // naranja = pesado
            : glm::vec4(0.9f,0.1f,0.1f,1.0f); // rojo = sobrecarga
    dibujarRect(MARGEN+BORDE, pesoY+BORDE, (BARRA_ANCHO-BORDE*2)*pesoPct, BARRA_ALTO-BORDE*2, colorPeso);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

bool Hud::proyectar(glm::vec3 pos3D, const glm::mat4& view, const glm::mat4& proj,
                    float& screenX, float& screenY) {
    glm::vec4 clip = proj * view * glm::vec4(pos3D, 1.0f);
    if (clip.w <= 0.0f) return false;
    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    if (ndc.z > 1.0f) return false;
    screenX = (ndc.x * 0.5f + 0.5f) * anchoVentana;
    screenY = (ndc.y * 0.5f + 0.5f) * altoVentana;
    return true;
}

void Hud::dibujarBarrasZombies(const std::vector<Zombie>& zombies,
                                const glm::mat4& view, const glm::mat4& proj) {
    if (zombies.empty()) return;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float BW = 40.0f, BH = 5.0f, BO = 1.0f, OY = 20.0f;

    for (const Zombie& z : zombies) {
        if (z.estaMuerto()) continue;
        if (z.getSaludPorcentaje() >= 1.0f) continue;
        glm::vec3 posHead = z.posicion + glm::vec3(0.0f, 1.5f, 0.0f);
        float sx, sy;
        if (!proyectar(posHead, view, proj, sx, sy)) continue;
        sy += OY;
        float bx = sx - BW * 0.5f;
        dibujarRect(bx, sy, BW, BH, glm::vec4(0.1f,0.1f,0.1f,0.85f));
        float pct = z.getSaludPorcentaje();
        glm::vec4 cv = pct > 0.5f ? glm::vec4(0.2f,0.8f,0.2f,1.0f) : glm::vec4(0.85f,0.15f,0.15f,1.0f);
        dibujarRect(bx+BO, sy+BO, (BW-BO*2)*pct, BH-BO*2, cv);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Hud::dibujarInventario(const Inventario& inv) {
    if (!inv.estaAbierto()) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float SLOT_TAM   = 52.0f;
    const float SLOT_PAD   = 4.0f;
    const float BORDE      = 2.0f;
    const float PANEL_PAD  = 12.0f;

    // Centrar el panel en la pantalla
    float panelAncho = Inventario::COLUMNAS * (SLOT_TAM + SLOT_PAD) + SLOT_PAD + PANEL_PAD * 2;
    float panelAlto  = Inventario::FILAS    * (SLOT_TAM + SLOT_PAD) + SLOT_PAD + PANEL_PAD * 2;
    float panelX     = (anchoVentana - panelAncho) * 0.5f;
    float panelY     = (altoVentana  - panelAlto)  * 0.5f;

    // Fondo del panel
    dibujarRect(panelX, panelY, panelAncho, panelAlto, glm::vec4(0.08f, 0.08f, 0.08f, 0.92f));

    // Slots
    for (int fila = 0; fila < Inventario::FILAS; fila++) {
        for (int col = 0; col < Inventario::COLUMNAS; col++) {
            int slot = fila * Inventario::COLUMNAS + col;

            float sx = panelX + PANEL_PAD + SLOT_PAD + col * (SLOT_TAM + SLOT_PAD);
            float sy = panelY + PANEL_PAD + SLOT_PAD + fila * (SLOT_TAM + SLOT_PAD);

            const Item& item = inv.getSlot(slot);
            bool seleccionado = (slot == inv.getSlotSeleccionado());

            // Fondo del slot
            glm::vec4 colorSlot(0.18f, 0.18f, 0.18f, 1.0f);
            if (seleccionado)
                colorSlot = glm::vec4(0.35f, 0.35f, 0.15f, 1.0f);  // amarillo oscuro
            dibujarRect(sx, sy, SLOT_TAM, SLOT_TAM, colorSlot);

            // Contenido del slot
            if (item.esValido()) {
                float r, g, b;
                DatosItem::getColor(item.tipo, r, g, b);

                // Relleno con color del item
                dibujarRect(sx + BORDE, sy + BORDE,
                            SLOT_TAM - BORDE*2, SLOT_TAM - BORDE*2,
                            glm::vec4(r, g, b, 0.85f));

                // Indicador de cantidad (barra pequeña abajo)
                if (item.stackMax > 1) {
                    float pct = (float)item.cantidad / item.stackMax;
                    dibujarRect(sx + BORDE, sy + BORDE,
                                (SLOT_TAM - BORDE*2) * pct, 4.0f,
                                glm::vec4(1.0f, 1.0f, 1.0f, 0.6f));
                }
            }

            // Borde del slot seleccionado
            if (seleccionado) {
                // Borde superior
                dibujarRect(sx, sy + SLOT_TAM - BORDE, SLOT_TAM, BORDE, glm::vec4(1.0f,0.9f,0.2f,1.0f));
                // Borde inferior
                dibujarRect(sx, sy, SLOT_TAM, BORDE, glm::vec4(1.0f,0.9f,0.2f,1.0f));
                // Borde izquierdo
                dibujarRect(sx, sy, BORDE, SLOT_TAM, glm::vec4(1.0f,0.9f,0.2f,1.0f));
                // Borde derecho
                dibujarRect(sx + SLOT_TAM - BORDE, sy, BORDE, SLOT_TAM, glm::vec4(1.0f,0.9f,0.2f,1.0f));
            }
        }
    }

    // Instrucciones en la parte inferior del panel
    // (texto no disponible sin librería de fuentes — por ahora solo el panel)

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}