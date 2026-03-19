# Hollow Earth — Registro de Progreso
> Actualizar este archivo después de cada sesión de desarrollo
> Desarrollado por: Lautaro ([@Food2001](https://github.com/Food2001)) + Claude (Anthropic)

---

## ESTADO ACTUAL DEL PROYECTO
**Fase:** Engine C++ funcional con terreno, jugador y cámara  
**Fecha de inicio:** Marzo 2026  
**Última actualización:** 19/03/2026

---

## PROTOTIPO EN GODOT 4 (Hollow Earth — versión aprendizaje)

### COMPLETADO ✅

- Cámara orbital Q/E + zoom + lerp
- Jugador con stamina, salud, ataque (raycast)
- Zombie con IA básica por distancia, barra de salud
- Mundo procedural por chunks (32x32), FastNoiseLite FBM
- Colores por altura: agua, arena, pasto, tierra, nieve
- Carga/descarga dinámica de chunks
- Spawn de zombies por chunk
- Inventario grilla 5x4, apilado de items

### PENDIENTE EN GODOT ❌
- Hotbar visible
- Items reales y loot
- Daño por caída
- Construcción básica
- Compatibilidad con mando

---

## ENGINE C++ — HOLLOW EARTH REAL

### ENTORNO DE DESARROLLO ✅
```
Compilador  → GCC 15.2.0 (MinGW-W64) en C:\mingw64
IDE         → VS Code + CMake Tools
SDL3        → C:\SDL3
GLAD        → C:\Hollow Earth\include\ y src\
GLM         → C:\Hollow Earth\include\glm\
FastNoiseLite → C:\Hollow Earth\include\FastNoiseLite.h
OpenGL      → 3.3.0 NVIDIA 353.54 — confirmado
CMake       → 4.3.0
Git         → 2.53.0
```

### COMANDO DE BUILD
```
Botón Build en barra inferior de VS Code
O: cmake --build build
```

### EJECUTAR
```
.\build\HollowEarth.exe
```

### ESTRUCTURA DEL PROYECTO
```
C:\Hollow Earth\
├── main.cpp
├── CMakeLists.txt
├── build\              ← ejecutable compilado aquí
├── src\
│   ├── glad.c
│   ├── shader.cpp
│   ├── camara.cpp
│   ├── terreno.cpp
│   └── jugador.cpp
└── include\
    ├── glad\
    ├── KHR\
    ├── glm\
    ├── FastNoiseLite.h
    ├── shader.h
    ├── camara.h
    ├── terreno.h
    └── jugador.h
```

### HITOS COMPLETADOS ✅
1. Hola Mundo C++
2. Ventana SDL3
3. OpenGL 3.3 + GLAD inicializado
4. Triángulo con shaders (vertex + fragment)
5. Cubo 3D con matrices MVP
6. Cámara orbital Q/E + zoom rueda del mouse
7. Terreno procedural FastNoiseLite con colores por altura
8. Código separado en archivos (shader/camara/terreno/jugador)
9. CMake configurado con botón Build en VS Code
10. Jugador (cubo amarillo) con gravedad y colisión con terreno
11. Movimiento WASD relativo a la cámara
12. Lerp suave de posición y ángulo de cámara
13. Terreno aleatorio con semilla aleatoria (rand)

### PRÓXIMO PASO 🔄
- Sistema de chunks (terreno infinito)
- Salto con espacio

### PENDIENTE EN ENGINE ❌
- Sistema de chunks
- Salto
- Múltiples entidades (futuros zombies)
- Shaders en archivos .glsl separados
- Carga de modelos .gltf
- Audio con OpenAL
- Sistema de mods con Lua + sol2
- IA con Python + pybind11
- Bullet Physics

---

## DECISIONES TÉCNICAS TOMADAS

| Decisión | Elección | Razón |
|----------|----------|-------|
| Renderer | SDL3 + OpenGL 3.3 | Liviano, compatible con hardware mínimo |
| Loader OpenGL | GLAD | Más moderno que GLEW, sin DLL extra |
| Audio | OpenAL | Open source, coherente con GPL |
| Scripting mods | Lua + sol2 | Más conocido, liviano |
| IA avanzada | Python + pybind11 | Mejores librerías de ML |
| Física | Bullet Physics (futuro) | Open source, probado |
| Ruido | FastNoiseLite (ahora) / FastNoise2 (futuro) | Simple primero |
| Ciudades | WFC + Prefabs | Híbrido calidad/infinito |
| Licencia | GPL v3 | Libertad con reciprocidad |
| Idioma código | Español | Equipo hispanohablante |
| Extensiones | .he* propias | Control total del formato |
| Build | CMake + MinGW | Estándar multiplataforma |

---

## NOTAS DE SESIÓN

### 19/03/2026 — Sesión 1 y 2
- Instalado entorno completo: MinGW, CMake, Git, SDL3, GLAD, GLM, FastNoiseLite
- Primer programa C++ compilado
- Ventana SDL3 + OpenGL 3.3 funcionando
- Triángulo RGB con shaders
- Cubo 3D girando con matrices MVP
- Cámara orbital con lerp suave
- Terreno procedural con colores por altura
- Jugador con gravedad y colisión con terreno
- Movimiento WASD relativo a cámara
- Código organizado en archivos separados
- CMake configurado

---

## CÓMO USAR ESTE ARCHIVO

Antes de cada sesión con Claude, pasá ambos archivos:
- `HOLLOW_EARTH_DESIGN.md` → contexto de diseño
- `HOLLOW_EARTH_PROGRESS.md` → estado actual

Claude los lee y retoma desde donde dejaste sin perder contexto.
