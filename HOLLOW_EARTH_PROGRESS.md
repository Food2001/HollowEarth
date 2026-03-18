# Hollow Earth — Registro de Progreso
> Actualizar este archivo después de cada sesión de desarrollo

---

## ESTADO ACTUAL DEL PROYECTO
**Fase:** Prototipo en Godot 4 + Planificación del engine C++  
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
  - Correr consume stamina
  - Estado agotado con velocidad reducida
  - Recuperación lenta
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

### EN PROGRESO 🔄
- Items de prueba en inventario (test básico funcionando)
- Ajuste visual del inventario

### PENDIENTE EN GODOT ❌
- Hotbar de 5 slots visible en pantalla
- Items reales en el mundo para recoger
- Sistema de loot en zombies al morir
- Distribuciones de loot (lua equivalente)
- Daño por caída
- Sistema de construcción básico
- Compatibilidad con mando (Input Actions ya preparadas)

---

## ENGINE C++ — HOLLOW EARTH REAL

### STACK DEFINIDO ✅
```
C++          → engine core
SDL2         → ventana e input
OpenGL       → rendering 3D
OpenAL       → audio
FastNoise2   → generación procedural
Bullet       → física
Lua + sol2   → sistema de mods
Python + pybind11 → IA avanzada
CMake        → compilación
```

### HERRAMIENTAS INSTALADAS
- [ ] MinGW (compilador GCC para Windows)
- [ ] VS Code con extensión C/C++
- [ ] CMake
- [ ] Git configurado con repositorio HollowEarth

### PENDIENTE — ENGINE ❌
- Todo — aún no iniciado
- Primer objetivo: ventana SDL2 + OpenGL funcionando
- Segundo objetivo: triángulo en pantalla (el "Hola Mundo" de OpenGL)

---

## DECISIONES TÉCNICAS TOMADAS

| Decisión | Elección | Razón |
|----------|----------|-------|
| Renderer | SDL2 + OpenGL | Liviano, compatible con hardware mínimo |
| Audio | OpenAL | Open source, coherente con GPL |
| Scripting mods | Lua + sol2 | Más conocido, liviano, comunidad PZ ya sabe |
| IA avanzada | Python + pybind11 | Mejores librerías de ML |
| Física | Bullet Physics | Open source, probado |
| Ruido | FastNoise2 | Más rápido disponible, SIMD |
| Ciudades | WFC + Prefabs | Híbrido calidad/infinito |
| Licencia | GPL v3 | Libertad con reciprocidad |
| Idioma código | Español | Equipo hispanohablante |
| Extensiones | .he* propias | Control total del formato |

---

## SISTEMAS DISEÑADOS (pendientes de implementar)

- [x] Generación procedural de terreno
- [x] Sistema de chunks
- [x] Sistema de infectados con arquetipos
- [x] Sistema de combate
- [x] Ramas de armas separadas
- [x] Artesanía de armas de fuego
- [x] Profesiones y traits profundos
- [x] Aptitudes finales (3 opciones)
- [x] Especialista vs Comodín
- [x] Sistema de salud física realista
- [x] Modos de inicio configurables
- [x] NPCs con IA por capas
- [x] Sistema de facciones dinámicas
- [x] Sistema de rumores
- [x] Simulación offscreen durante sueño
- [x] Deterioro del mundo con el tiempo
- [x] Desastres naturales por bioma
- [x] Ciclo día/noche y estaciones
- [x] Sistema de mods Lua + Python
- [x] Editor de mapas separado
- [x] Logo de facción personalizable

---

## NOTAS DE SESIÓN

### Marzo 2026 — Sesión inicial
- Prototipo Godot funcional con terreno, jugador, zombie, inventario básico
- Documento de diseño completo creado
- Repositorio GitHub creado
- Próximo paso: instalar MinGW y hacer primer programa C++

---

## CÓMO USAR ESTE ARCHIVO

Antes de cada sesión con Claude, pasá ambos archivos:
- HOLLOW_EARTH_DESIGN.md → contexto de diseño
- HOLLOW_EARTH_PROGRESS.md → estado actual

Claude los lee y retoma desde donde dejaste sin perder contexto.

