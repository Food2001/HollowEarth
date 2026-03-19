# Hollow Earth — Registro de Progreso
> Actualizar este archivo después de cada sesión de desarrollo
> Desarrollado por: Lautaro ([@Food2001](https://github.com/Food2001)) + Claude (Anthropic)

---

## ESTADO ACTUAL DEL PROYECTO
**Fase:** Prototipo en Godot 4 + Engine C++ iniciado  
**Fecha de inicio:** Marzo 2026

---

## PROTOTIPO EN GODOT 4 (Hollow Earth — versión aprendizaje)

### COMPLETADO ✅

**Cámara orbital:**
- Rotación con Q/E en pasos de 90°
- Zoom con rueda del mouse
- Suavidad con lerp
- Siempre detrás del jugador
- Angulo público para sincronizar movimiento

**Jugador (CharacterBody3D):**
- Movimiento relativo a la cámara
- Sistema de stamina completo
- Sistema de salud (0 a 3)
  - Regeneración lenta tras 5 segundos sin daño
  - Muerte recarga la escena
- Sistema de ataque cuerpo a cuerpo
  - Click izquierdo del mouse
  - Raycast frente al jugador
  - Rango 2.5 unidades
  - Cooldown 0.6 segundos
  - Daño 1.0 por golpe

**Zombie (CharacterBody3D):**
- Persecución directa al jugador (sin NavMesh)
- Detección por distancia (15 unidades)
- Deja de perseguir a más de 20 unidades
- Se duerme a más de 40 unidades
- Sistema de salud (3 golpes para morir)
- Barra de salud Label3D con billboard
  - Aparece al recibir daño
  - Desaparece a los 3 segundos
  - Verde → amarillo → rojo según salud
- Daño al jugador con cooldown de 1.5 segundos
- Grupo: "zombie"

**Mundo procedural:**
- Generación por chunks (32x32 unidades)
- Render distance: 3 chunks en cada dirección (7x7 = 49 chunks)
- HeightMapShape3D para colisión estable
- FastNoiseLite con FBM (4 octavas)
- Colores por altura: agua, arena, pasto, tierra, nieve
- Carga y descarga dinámica de chunks
- Borrado diferido para evitar caídas al caminar

**Spawn de zombies:**
- Al generar cada chunk
- 0 a 2 zombies por chunk
- 30% de probabilidad por chunk
- Sin spawn en chunk inicial (0,0)
- Distancia mínima al jugador de 20 unidades
- Spawn diferido con call_deferred
- Persisten aunque el chunk se descargue
- IA activa solo dentro de 60 unidades

**Inventario:**
- Grilla 5x4 = 20 slots
- Se abre con Tab
- Slots como Panel con TextureRect y Label
- Sistema de apilado de items
- Funciones: agregar_item, remover_item, tiene_item
- Items de prueba funcionando (Piedra x5, Madera x3)

### PENDIENTE EN GODOT ❌
- Hotbar de 5 slots visible en pantalla
- Items reales en el mundo para recoger
- Sistema de loot en zombies al morir
- Daño por caída
- Sistema de construcción básico
- Compatibilidad con mando

---

## ENGINE C++ — HOLLOW EARTH REAL

### ENTORNO DE DESARROLLO ✅
```
Compilador  → GCC 15.2.0 (MinGW-W64)
IDE         → VS Code
SDL3        → instalado en C:\SDL3
GLAD        → instalado en C:\Hollow Earth\include y src
OpenGL      → 3.3.0 NVIDIA 353.54 — confirmado funcionando
CMake       → 4.3.0
Git         → 2.53.0
```

### COMPLETADO EN ENGINE ✅
- Primer programa C++ compilado y ejecutado
- Ventana SDL3 funcionando
- Contexto OpenGL 3.3 creado
- GLAD inicializado correctamente
- Color de fondo verde renderizando (primer frame de Hollow Earth)
- VSync activado

### PRÓXIMO PASO 🔄
- Triángulo con OpenGL (shaders + VAO/VBO)

### PENDIENTE EN ENGINE ❌
- Triángulo con shaders
- Sistema de cámara 3D
- Carga de modelos
- Sistema de chunks en C++
- Generación procedural con FastNoise2
- Sistema de físicas con Bullet
- Audio con OpenAL
- Sistema de mods con Lua + sol2
- IA con Python + pybind11

---

## DECISIONES TÉCNICAS TOMADAS

| Decisión | Elección | Razón |
|----------|----------|-------|
| Renderer | SDL3 + OpenGL 3.3 | Liviano, compatible con hardware mínimo |
| Loader OpenGL | GLAD | Más moderno que GLEW, sin DLL extra |
| Audio | OpenAL | Open source, coherente con GPL |
| Scripting mods | Lua + sol2 | Más conocido, liviano |
| IA avanzada | Python + pybind11 | Mejores librerías de ML |
| Física | Bullet Physics | Open source, probado |
| Ruido | FastNoise2 | Más rápido disponible |
| Ciudades | WFC + Prefabs | Híbrido calidad/infinito |
| Licencia | GPL v3 | Libertad con reciprocidad |
| Idioma código | Español | Equipo hispanohablante |
| Extensiones | .he* propias | Control total del formato |

---

## ESTRUCTURA DEL PROYECTO C++

```
C:\Hollow Earth\
├── main.cpp          → entrada principal
├── hollow_earth.exe  → ejecutable compilado
├── SDL3.dll          → librería SDL3
├── src\
│   └── glad.c        → loader de OpenGL
└── include\
    ├── glad\
    │   └── glad.h
    └── KHR\
        └── khrplatform.h

Librerías externas:
├── C:\SDL3\          → SDL3
└── C:\mingw64\       → compilador GCC
```

## COMANDO DE COMPILACIÓN ACTUAL
```
g++ main.cpp src/glad.c -o hollow_earth.exe -I include -I C:\SDL3\include -L C:\SDL3\lib -lSDL3
```

---

## NOTAS DE SESIÓN

### Marzo 2026 — Sesión 1
- Prototipo Godot funcional con terreno, jugador, zombie, inventario básico
- Documento de diseño completo creado
- Repositorio GitHub creado: github.com/Food2001/HollowEarth
- Instalado: MinGW GCC 15.2.0, CMake 4.3.0, Git, SDL3, GLAD
- Primer programa C++ compilado
- Ventana SDL3 + OpenGL 3.3 funcionando
- Primer frame verde renderizado — Hollow Earth engine iniciado

---

## CÓMO USAR ESTE ARCHIVO

Antes de cada sesión con Claude, pasá ambos archivos:
- HOLLOW_EARTH_DESIGN.md → contexto de diseño
- HOLLOW_EARTH_PROGRESS.md → estado actual

Claude los lee y retoma desde donde dejaste sin perder contexto.
