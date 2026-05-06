#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>

#include "shader.h"
#include "camara.h"
#include "mundo.h"
#include "jugador.h"
#include "textura.h"
#include "hud.h"
#include "skybox.h"
#include "zombie.h"
#include "particula.h"
#include "inventario.h"
#include "gestor.h"

const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA  = 600;

bool ESCALONADO_ACTIVO = false;

std::string leerArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    std::stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

void limpiarMuertos(std::vector<Zombie>& zombies, SistemaParticulas& particulas, Jugador& jugador) {
    std::vector<size_t> indicesMuertos;
    for (size_t i = 0; i < zombies.size(); i++) {
        if (zombies[i].estaMuerto()) {
            particulas.emitirMuerteZombie(zombies[i].posicion);
            if ((rand() % 100) < 70)
                jugador.inventario.agregarItem(DatosItem::crearVenda());
            if ((rand() % 100) < 20)
                jugador.inventario.agregarItem(DatosItem::crearKitMedico());
            indicesMuertos.push_back(i);
        }
    }
    for (auto it = indicesMuertos.rbegin(); it != indicesMuertos.rend(); ++it) {
        zombies.erase(zombies.begin() + *it);
    }
}

void aplicarAtaqueCono(Jugador& jugador, std::vector<Zombie>& zombies) {
    float radRotacion = glm::radians(jugador.rotacion);
    glm::vec3 frente  = glm::normalize(glm::vec3(sin(radRotacion), 0.0f, cos(radRotacion)));
    float mitadCono   = glm::radians(Jugador::ANGULO_CONO * 0.5f);
    for (auto& z : zombies) {
        if (z.estaMuerto()) continue;
        glm::vec3 diff = z.posicion - jugador.posicion;
        diff.y = 0.0f;
        float dist = glm::length(diff);
        if (dist > Jugador::ALCANCE_ATAQUE) continue;
        if (dist < 0.001f) { z.recibirDanio(Jugador::DANIO_ATAQUE, frente); continue; }
        glm::vec3 dirZ   = glm::normalize(diff);
        float     cosAng = glm::dot(frente, dirZ);
        if (cosAng >= cos(mitadCono))
            z.recibirDanio(Jugador::DANIO_ATAQUE, frente);
    }
}

void imprimirDebug(float fps, float delta, const Gestor& gestor, int totalZombies, bool escalonado) {
    auto& hist = gestor.getHistorial();
    auto& tirones = gestor.getTirones();

    std::cout << "\n======================================\n";
    std::cout << "  HOLLOW EARTH — DEBUG (Últimos 5s)\n";
    std::cout << "======================================\n";
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "  FPS: " << fps << " | Delta: " << (delta * 1000.0f) << "ms\n";
    std::cout << "  Modo: " << (escalonado ? "CON escalonado [G]" : "SIN escalonado [G]") << "\n";
    std::cout << "--------------------------------------\n";
    std::cout << "  ENTIDADES\n";
    std::cout << "  Zombies totales:  " << totalZombies << "\n";
    std::cout << "  Zombies activos:  " << gestor.getStats().zombiesActivos << "\n";
    std::cout << "  Zombies dormidos: " << gestor.getStats().zombiesDormidos << "\n";
    std::cout << "  Chunks cargados:  " << gestor.getStats().chunksActivos << "\n";
    std::cout << "--------------------------------------\n";
    std::cout << "  HISTORIAL FPS (últimos 5s)\n  ";
    for (float f : hist) std::cout << (int)f << " ";
    std::cout << "\n";
    std::cout << "  Min: " << gestor.getFPSMinimo()
              << " | Max: " << gestor.getFPSMaximo()
              << " | Prom: " << gestor.getFPSPromedio() << "\n";

    if (!tirones.empty()) {
        std::cout << "--------------------------------------\n";
        std::cout << "  TIRONES DETECTADOS (" << tirones.size() << ")\n";
        for (const auto& t : tirones) {
            std::cout << "  ⚠ Frame " << t.frame
                      << " | " << (t.delta * 1000.0f) << "ms"
                      << " | Z:" << t.zombies
                      << " | Chunks:" << t.chunks << "\n";
        }
    }
    std::cout << "======================================\n";
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(nullptr));

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "Error SDL3: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* ventana = SDL_CreateWindow("Hollow Earth", ANCHO_VENTANA, ALTO_VENTANA, SDL_WINDOW_OPENGL);
    if (!ventana) { std::cout << "Error ventana: " << SDL_GetError() << std::endl; return 1; }

    SDL_GLContext contexto = SDL_GL_CreateContext(ventana);
    SDL_GL_SetSwapInterval(1);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Tecla G — alternar escalonado ON/OFF\n";
    std::cout << "Consola se actualiza cada 5 segundos.\n\n";

    std::string vertexCode   = leerArchivo("shaders/vertex.glsl");
    std::string fragmentCode = leerArchivo("shaders/fragment.glsl");
    Shader shader(vertexCode.c_str(), fragmentCode.c_str());

    Camara            camara;
    Mundo             mundo;
    mundo.inicializar();
    Jugador           jugador;
    Hud               hud(ANCHO_VENTANA, ALTO_VENTANA);
    Skybox            skybox("assets/textures/skybox");
    SistemaParticulas particulas;
    Gestor            gestor;

    float salud        = 1.0f;
    bool  muerto       = false;
    float tiempoMuerto = 0.0f;

    std::vector<Zombie> zombies;

    float tiempoSinZombies = 0.0f;
    int   numeroOleada     = 1;
    static constexpr float TIEMPO_NUEVA_OLEADA = 30.0f;

    std::cout << "Cargando texturas..." << std::endl;
    Textura texturaPasto("assets/textures/pasto.png", "diffuse");
    Textura texturaArena("assets/textures/arena.png", "diffuse");
    Textura texturaRoca ("assets/textures/roca.png",  "diffuse");
    Textura texturaNieve("assets/textures/nieve.png", "diffuse");
    Textura texturaAgua ("assets/textures/agua.png",  "diffuse");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.6f, 0.7f, 1.0f);

    float tiempoAnterior = (float)SDL_GetTicks() / 1000.0f;
    float deltaActual    = 0.0f;

    float tiempoDebug   = 0.0f;
    float tiempoTitulo   = 0.0f;
    int   frameCountDebug = 0;
    int   frameCountTitulo = 0;

    glm::vec3 luzPosicion(100.0f, 100.0f, 100.0f);
    glm::vec3 luzColor(1.0f, 0.95f, 0.9f);
    float     luzIntensidad     = 1.2f;
    glm::vec3 ambientColor(0.8f, 0.85f, 1.0f);
    float     ambientIntensidad = 0.3f;
    glm::vec3 nieblaColor(0.5f, 0.6f, 0.7f);
    float     nieblaDensidad    = 0.008f;
    bool      nieblaHabilitada  = true;

    // Cachear uniform locations para evitar búsquedas por string cada frame
    GLint locModel      = glGetUniformLocation(shader.id, "model");
    GLint locColorSolido = glGetUniformLocation(shader.id, "colorSolido");
    GLint locUsarTextura = glGetUniformLocation(shader.id, "usarTextura");
    GLint locUsarMulti   = glGetUniformLocation(shader.id, "usarMultiTextura");
    GLint locAlpha       = glGetUniformLocation(shader.id, "alpha");
    GLint locUsarInstancia = glGetUniformLocation(shader.id, "usarInstancia");

    bool corriendo = true;
    SDL_Event evento;

    while (corriendo) {
        float tiempoActual = (float)SDL_GetTicks() / 1000.0f;
        float delta        = tiempoActual - tiempoAnterior;
        tiempoAnterior     = tiempoActual;
        if (delta > 0.1f) delta = 0.1f;
        deltaActual = delta;

        gestor.iniciarFrame();
        tiempoDebug  += delta;
        tiempoTitulo += delta;
        frameCountDebug++;
        frameCountTitulo++;

        // Eventos (sin cambios)
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) corriendo = false;
            if (evento.type == SDL_EVENT_KEY_DOWN) {
                if (evento.key.key == SDLK_Q)      camara.rotarIzquierda();
                if (evento.key.key == SDLK_E)      camara.rotarDerecha();
                if (evento.key.key == SDLK_ESCAPE) corriendo = false;
                if (evento.key.key == SDLK_TAB)    jugador.inventario.toggleAbierto();

                if (evento.key.key == SDLK_G) {
                    ESCALONADO_ACTIVO = !ESCALONADO_ACTIVO;
                    std::cout << "Escalonado: " << (ESCALONADO_ACTIVO ? "ON" : "OFF") << "\n";
                }

                if (evento.key.key == SDLK_SPACE && !jugador.inventario.estaAbierto())
                    jugador.saltar();

                if (jugador.inventario.estaAbierto()) {
                    int slot = jugador.inventario.getSlotSeleccionado();
                    if (slot < 0) slot = 0;
                    if (evento.key.key == SDLK_RIGHT)
                        jugador.inventario.setSlotSeleccionado((slot+1) % Inventario::MAX_SLOTS);
                    if (evento.key.key == SDLK_LEFT)
                        jugador.inventario.setSlotSeleccionado((slot-1+Inventario::MAX_SLOTS) % Inventario::MAX_SLOTS);
                    if (evento.key.key == SDLK_DOWN)
                        jugador.inventario.setSlotSeleccionado((slot+Inventario::COLUMNAS) % Inventario::MAX_SLOTS);
                    if (evento.key.key == SDLK_UP)
                        jugador.inventario.setSlotSeleccionado((slot-Inventario::COLUMNAS+Inventario::MAX_SLOTS) % Inventario::MAX_SLOTS);
                    if (evento.key.key == SDLK_F) {
                        float curacion = jugador.usarItemSeleccionado();
                        if (curacion > 0.0f)
                            salud = std::min(1.0f, salud + curacion);
                    }
                }
            }
            if (evento.type == SDL_EVENT_MOUSE_WHEEL)
                camara.hacerZoom(evento.wheel.y);

            if (evento.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                evento.button.button == SDL_BUTTON_LEFT &&
                !muerto && !jugador.inventario.estaAbierto()) {
                if (jugador.intentarAtacar()) {
                    aplicarAtaqueCono(jugador, zombies);
                    limpiarMuertos(zombies, particulas, jugador);
                }
            }
        }

        // Movimiento
        if (!muerto && !jugador.inventario.estaAbierto()) {
            const bool* teclas = SDL_GetKeyboardState(nullptr);
            glm::vec3 direccion(0.0f);
            glm::vec3 frente  = camara.getDireccionFrente();
            glm::vec3 derecha = glm::normalize(glm::cross(frente, glm::vec3(0.0f,1.0f,0.0f)));
            if (teclas[SDL_SCANCODE_W]) direccion += frente;
            if (teclas[SDL_SCANCODE_S]) direccion -= frente;
            if (teclas[SDL_SCANCODE_A]) direccion -= derecha;
            if (teclas[SDL_SCANCODE_D]) direccion += derecha;
            bool intentaCorrer = teclas[SDL_SCANCODE_LSHIFT] || teclas[SDL_SCANCODE_RSHIFT];
            jugador.actualizarAtaque(delta);
            jugador.actualizarStamina(delta, intentaCorrer);
            jugador.mover(direccion, delta, intentaCorrer);
            jugador.actualizarFisica(delta, mundo);
        }

        mundo.actualizar(jugador.posicion.x, jugador.posicion.z);

        for (const auto& [cx, cz] : mundo.getChunksNuevos()) {
            mundo.generarZombiesChunk(cx, cz, zombies);
        }

        if (!muerto) {
            if (zombies.empty()) {
                tiempoSinZombies += delta;
                if (tiempoSinZombies >= TIEMPO_NUEVA_OLEADA) {
                    tiempoSinZombies = 0.0f;
                    numeroOleada++;
                    int cantidad = 5 + numeroOleada * 2;
                    for (int i = 0; i < cantidad; i++) {
                        float ang  = (rand() / (float)RAND_MAX) * 360.0f;
                        float rad  = glm::radians(ang);
                        float radio = 15.0f + (rand() / (float)RAND_MAX) * 25.0f;
                        float px   = jugador.posicion.x + sin(rad) * radio;
                        float pz   = jugador.posicion.z + cos(rad) * radio;
                        float py   = std::max(3.0f, mundo.obtenerAltura(px, pz) + 3.0f);
                        zombies.emplace_back(glm::vec3(px, py, pz));
                    }
                    std::cout << "=== OLEADA " << numeroOleada << " — " << cantidad << " zombies ===\n";
                }
            } else {
                tiempoSinZombies = 0.0f;
            }
        }

        // Actualizar zombies (con perfilado, sin cambios en lógica)
        int zombiesActivos = 0;
        int zombiesDormidos = 0;
        for (size_t i = 0; i < zombies.size(); i++) {
            if (zombies[i].estaMuerto()) continue;
            bool debeActualizar = !ESCALONADO_ACTIVO || gestor.debeActualizar(zombies[i].posicion, jugador.posicion, (int)i);
            if (debeActualizar) {
                zombies[i].actualizar(delta, jugador.posicion, mundo);
                zombiesActivos++;
            } else {
                zombies[i].actualizarFisica(delta, mundo);
                zombiesDormidos++;
            }
            salud -= zombies[i].getDanioFrame(delta) / 100.0f;
        }
        salud = std::max(0.0f, salud);
        if (salud <= 0.0f) muerto = true;

        gestor.getStats().zombiesActivos = zombiesActivos;
        gestor.getStats().zombiesDormidos = zombiesDormidos;
        gestor.getStats().zombiesTotales = (int)zombies.size();

        limpiarMuertos(zombies, particulas, jugador);

        // Respawn
        if (muerto) {
            tiempoMuerto += delta;
            if (tiempoMuerto >= 3.0f) {
                salud              = 1.0f;
                muerto             = false;
                tiempoMuerto       = 0.0f;
                numeroOleada       = 1;
                jugador.posicion   = glm::vec3(0.0f, 10.0f, 0.0f);
                jugador.velocidadY = 0.0f;
                zombies.clear();
                particulas.actualizar(999.0f); // reset partículas
            }
        }

        particulas.actualizar(delta);
        camara.objetivo = jugador.posicion;

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camara.getViewConColision(mundo);
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)ANCHO_VENTANA / (float)ALTO_VENTANA,
            0.1f, 1000.0f
        );
        glm::mat4 viewProj = projection * view;

        skybox.dibujar(view, projection);

        shader.usar();
        // Configurar iluminación/niebla una vez (solo cambian si se modifican, pero aquí son fijas)
        shader.setVec3("luzPosicion",        luzPosicion);
        shader.setVec3("luzColor",           luzColor);
        shader.setFloat("luzIntensidad",     luzIntensidad);
        shader.setVec3("ambientColor",       ambientColor);
        shader.setFloat("ambientIntensidad", ambientIntensidad);
        shader.setVec3("nieblaColor",        nieblaColor);
        shader.setFloat("nieblaDensidad",    nieblaDensidad);
        shader.setBool("nieblaHabilitada",   nieblaHabilitada);
        shader.setMat4("view",       glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        // =============== TERRENO ===============
        shader.setFloat("alpha",           1.0f);
        shader.setBool("usarMultiTextura", true);
        shader.setBool("usarTextura",      false);
        shader.setVec2("uvOffset",         glm::vec2(0.0f, 0.0f));
        shader.setInt("texturaArena", 1);
        shader.setInt("texturaPasto", 2);
        shader.setInt("texturaRoca",  3);
        shader.setInt("texturaNieve", 4);
        glm::mat4 modelTerreno = glm::mat4(1.0f);
        glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelTerreno));
        glUniform1i(locUsarInstancia, GL_FALSE);
        texturaArena.usar(1);
        texturaPasto.usar(2);
        texturaRoca.usar(3);
        texturaNieve.usar(4);
        mundo.dibujar(viewProj, nullptr);
        glUniform1i(locUsarMulti, GL_FALSE);
        shader.setVec2("uvOffset", glm::vec2(0.0f, 0.0f));

        // =============== ÁRBOLES (INSTANCIADO) ===============
        shader.setFloat("alpha", 1.0f);
        glUniform1i(locUsarInstancia, GL_TRUE);   // Solo para árboles
        mundo.dibujarArboles(shader, viewProj);
        glUniform1i(locUsarInstancia, GL_FALSE);

        // =============== JUGADOR ===============
        shader.setFloat("alpha", 1.0f);
        glm::mat4 modelJugador = glm::mat4(1.0f);
        modelJugador = glm::translate(modelJugador, jugador.posicion);
        modelJugador = glm::rotate(modelJugador, glm::radians(jugador.rotacion),
                                   glm::vec3(0.0f,1.0f,0.0f));
        glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelJugador));
        glUniform1i(locUsarTextura, GL_FALSE);
        glUniform3fv(locColorSolido, 1, glm::value_ptr(
            jugador.estaAtacando() ? glm::vec3(1.0f,1.0f,1.0f) : glm::vec3(1.0f,0.85f,0.0f)));
        jugador.dibujar();

        // =============== ZOMBIES (NUEVO: INSTANCIADO) ===============
        shader.setFloat("alpha", 1.0f);
        glUniform1i(locUsarTextura, GL_FALSE);
        // Preparar datos de instancias: matrices + colores
        static std::vector<glm::mat4> matricesZombies;
        static std::vector<glm::vec3> coloresZombies;
        matricesZombies.clear();
        coloresZombies.clear();

        for (const auto& z : zombies) {
            if (z.estaMuerto()) continue;
            // Frustum culling opcional (podemos agregarlo luego, por ahora solo distancia)
            // Aquí podríamos implementar un culling simple, pero lo dejamos para que no rompa nada.
            glm::mat4 mz = glm::translate(glm::mat4(1.0f), z.posicion);
            mz = glm::rotate(mz, glm::radians(z.rotacion), glm::vec3(0.0f,1.0f,0.0f));
            matricesZombies.push_back(mz);

            glm::vec3 colorZ;
            switch (z.estado) {
                case EstadoZombie::IDLE:         colorZ = glm::vec3(0.6f,0.1f,0.1f); break;
                case EstadoZombie::PERSIGUIENDO: colorZ = glm::vec3(0.9f,0.1f,0.1f); break;
                case EstadoZombie::ATACANDO:     colorZ = glm::vec3(1.0f,0.3f,0.0f); break;
            }
            if (z.tiempoKnockback > 0.0f)
                colorZ = glm::mix(colorZ, glm::vec3(1.0f), 0.7f);
            coloresZombies.push_back(colorZ);
        }

        if (!matricesZombies.empty()) {
            // Dibujar todos los zombies con una sola llamada instanciada
            Zombie::dibujarInstanciado(matricesZombies, coloresZombies, shader);
        }

        // =============== PARTÍCULAS ===============
        particulas.dibujar(shader);

        // =============== AGUA ===============
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        mundo.dibujarAgua(shader, viewProj, &texturaAgua);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // =============== HUD ===============
        hud.dibujar(salud, jugador.getStaminaPorcentaje(),
                    jugador.estaAgotado(), jugador.inventario.getPesoPorcentaje());
        hud.dibujarBarrasZombies(zombies, view, projection);
        hud.dibujarInventario(jugador.inventario);

        if (tiempoTitulo >= 1.0f) {
            float fpsTitulo = (float)frameCountTitulo / tiempoTitulo;
            std::string titulo = "H.E. | FPS: " + std::to_string((int)fpsTitulo)
                               + " | Z: " + std::to_string(zombies.size())
                               + " | Oleada: " + std::to_string(numeroOleada)
                               + (ESCALONADO_ACTIVO ? " [ESC ON]" : " [ESC OFF]")
                               + (jugador.estaAgotado() ? " [AGOTADO]" : "");
            SDL_SetWindowTitle(ventana, titulo.c_str());
            tiempoTitulo = 0.0f;
            frameCountTitulo = 0;
        }

        if (tiempoDebug >= 5.0f) {
            float fpsPromedio = (float)frameCountDebug / tiempoDebug;
            gestor.registrarFPS(fpsPromedio);
            gestor.registrarDelta(delta, (int)zombies.size(), mundo.getCantidadChunks());
            gestor.getStats().chunksActivos = mundo.getCantidadChunks();
            imprimirDebug(fpsPromedio, deltaActual, gestor, (int)zombies.size(), ESCALONADO_ACTIVO);
            tiempoDebug = 0.0f;
            frameCountDebug = 0;
        }

        SDL_GL_SwapWindow(ventana);
    }

    SDL_GL_DestroyContext(contexto);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return 0;
}