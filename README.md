# Hollow Earth

> Survival zombie de mundo abierto 3D, open source y moddeable.

![License](https://img.shields.io/badge/license-GPL%20v3-blue)
![Status](https://img.shields.io/badge/estado-en%20desarrollo-yellow)
![Language](https://img.shields.io/badge/lenguaje-C%2B%2B-orange)

---

## ¿Qué es Hollow Earth?

Hollow Earth es un juego de supervivencia zombie en mundo abierto 3D isométrico,
inspirado en Project Zomboid, Unturned y Minecraft, pero con sistemas más profundos,
mejor optimización y código completamente abierto bajo GPL v3.

Un virus de origen antiguo — atrapado millones de años en el permafrost — fue
descubierto por error. El sistema inmune humano nunca lo vio. No había defensa.

---

## Características principales

- **Mundo procedural infinito** — terreno, biomas, cuevas, ciudades generadas
- **Infectados con memoria residual** — cada zombie conserva habilidades de su vida anterior
- **Sistema de facciones dinámicas** — grupos que aprenden, crecen y colapsan
- **Simulación offscreen** — el mundo evoluciona mientras dormís
- **Deterioro del mundo** — las ciudades cambian con el tiempo y los conflictos
- **Desastres naturales** — según bioma, altura y coordenadas
- **Combate rápido y variado** — ramas de armas separadas con progresión única
- **Profesiones y traits profundos** — habilidades únicas permanentes
- **Modos de inicio configurables** — desde el Día 1 hasta 10+ años después
- **Completamente moddeable** — Lua para mecánicas, Python para IA

---

## Stack tecnológico

| Componente | Tecnología |
|------------|------------|
| Engine | C++ |
| Rendering | SDL2 + OpenGL |
| Física | Bullet Physics |
| Audio | OpenAL |
| Generación procedural | FastNoise2 |
| Sistema de mods | Lua + sol2 |
| IA avanzada | Python + pybind11 |
| Compilación | CMake |

---

## Hardware mínimo objetivo

| Componente | Mínimo |
|------------|--------|
| CPU | Dual core 3GHz |
| RAM | 8GB |
| GPU | 1GB VRAM, OpenGL 4.0+ |
| OS | Windows 10 64-bit |

**Filosofía:** Si corre en el hardware de desarrollo, corre en todo.

---

## Sistema de mods

Hollow Earth está diseñado desde el primer día para ser moddeable:

- **Lua** — items, mecánicas, eventos, facciones, diálogos
- **Python** — módulos de IA custom, modelos de ML propios
- **Código fuente completo** — sin restricciones, GPL v3
- **Editor de mapas** — herramienta separada también open source

---

## Extensiones de archivo

```
.hemap      → mapas del mundo
.hesave     → partidas guardadas
.hemod      → paquetes de mods
.heprefab   → prefabs del editor
.heconfig   → configuración
```

---

## Estado del proyecto

Actualmente en fase de prototipo en Godot 4 y planificación del engine C++.

Ver [HOLLOW_EARTH_PROGRESS.md](HOLLOW_EARTH_PROGRESS.md) para el estado detallado.  
Ver [HOLLOW_EARTH_DESIGN.md](HOLLOW_EARTH_DESIGN.md) para el documento de diseño completo.

---

## Licencia

GNU General Public License v3.0 — ver [LICENSE](LICENSE) para más detalles.

El código es libre. Si lo usás y modificás, tu versión también debe ser libre.

---

## Contribuir

El proyecto aún no acepta contribuciones externas — está en etapa muy temprana.
Cuando el engine base esté funcional, se abrirá a la comunidad.
