# Hollow Earth — Registro de Progreso
> Actualizar este archivo después de cada sesión de desarrollo
> Desarrollado por: Lautaro ([@Food2001](https://github.com/Food2001)) + Claude (Anthropic)

---

## ESTADO ACTUAL DEL PROYECTO
**Fase:** Engine C++ funcional — prototipo jugable con mundo, combate e inventario  
**Fecha de inicio:** Marzo 2026  
**Última actualización:** 21/03/2026

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
Compilador    → GCC 15.2.0 (MinGW-W64) en C:\mingw64
IDE           → VS Code + CMake Tools
SDL3          → C:\SDL3
SDL3_image    → C:\SDL3_image  (64-bit, para carga de PNG/BMP)
GLAD          → C:\Hollow Earth\include\ y src\
GLM           → C:\Hollow Earth\include\glm\
FastNoiseLite → C:\Hollow Earth\include\FastNoiseLite.h
OpenGL        → 3.3.0 NVIDIA 353.54 — confirmado
CMake         → 4.3.0
Git           → 2.53.0
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
├── recursos.rc                  ← ícono del .exe
├── shaders\
│   ├── vertex.glsl
│   └── fragment.glsl
├── assets\
│   └── textures\
│       ├── pasto.png
│       ├── arena.png
│       ├── roca.png
│       ├── nieve.png
│       ├── agua.png             ← convertida a RGB limpio (driver viejo)
│       └── skybox\
│           ├── clouds1_east.bmp
│           ├── clouds1_west.bmp
│           ├── clouds1_up.bmp
│           ├── clouds1_down.bmp
│           ├── clouds1_south.bmp
│           └── clouds1_north.bmp
├── build\                       ← ejecutable compilado aquí
├── src\
│   ├── glad.c
│   ├── shader.cpp
│   ├── camara.cpp
│   ├── terreno.cpp
│   ├── jugador.cpp
│   ├── mundo.cpp
│   ├── textura.cpp
│   ├── hud.cpp
│   ├── agua.cpp
│   ├── skybox.cpp
│   ├── zombie.cpp
│   ├── particula.cpp
│   └── inventario.cpp
└── include\
    ├── glad\
    ├── KHR\
    ├── glm\
    ├── FastNoiseLite.h
    ├── shader.h
    ├── camara.h
    ├── terreno.h
    ├── jugador.h
    ├── mundo.h
    ├── textura.h
    ├── hud.h
    ├── agua.h
    ├── skybox.h
    ├── zombie.h
    ├── particula.h
    └── inventario.h
```

---

## HITOS COMPLETADOS ✅

### ENGINE BASE
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

### MUNDO
14. Sistema de chunks dinámicos — carga/descarga según posición del jugador
15. Terreno con normales por vértice suavizadas (iluminación correcta)
16. Generación de altura mejorada — llanuras amplias, colinas suaves, montañas ocasionales
    - pow(n, 1.8f) aplana zonas bajas, exagera solo cumbres altas
    - Zonas negativas = depresiones para agua
17. Árboles procedurales por chunk — tronco marrón + copa verde, semilla determinista
    - Solo en zonas h > 0.5 y h < 12 (llanuras/colinas, no agua ni cumbres)
    - 0 a 12 árboles por chunk, tamaño variable
18. Agua semitransparente — plano en y=0 sobre chunks con zonas negativas
    - Textura animada con desplazamiento UV en el tiempo
    - Alpha 0.72, dibujada al final con glDepthMask(GL_FALSE)
19. Skybox con cubemap de 6 caras BMP (clouds1_north/south/east/west/up/down)
    - Shader propio sin traslación, glDepthFunc(GL_LEQUAL)
    - Se dibuja primero, detrás de todo

### RENDER Y SHADERS
20. Shaders en archivos .glsl separados, leídos en runtime
21. Iluminación difusa + ambiental (Phong simplificado)
    - Luz direccional con posición configurable
    - Sombras suaves con smoothstep (transición suave luz/sombra)
22. Niebla exponencial — `exp(-dist * densidad)`, mezclada con mix()
    - Densidad: 0.008 (reducida respecto al original)
23. Multitextura por altura en el fragment shader:
    - Arena: h < 3 (zonas bajas cerca del agua)
    - Pasto: h 0–8 (llanuras y colinas bajas)
    - Roca: h 5–14 + pendientes pronunciadas (dot(normal, up) < 0.6)
    - Nieve: h > 10 (cumbres)
    - Transiciones suaves con mezcla interpolada
    - Pendientes detectadas por normal → siempre roca independiente de altura
24. UV offset en vertex shader para animación del agua
25. Uniform `alpha` en fragment shader para transparencia del agua
26. Uniform `colorSolido` para objetos sin textura (jugador, zombies, árboles)
27. Uniform `usarMultiTextura` para separar terreno del resto

### CÁMARA
28. Cámara orbital isométrica — siempre mira al jugador
29. Rotación Q/E en pasos de 90°, lerp suave
30. Zoom con rueda del mouse, rango 0.3x a 3.0x
31. Colisión con terreno — barrido horizontal en 20 pasos
    - Detecta si el terreno supera la altura de la cámara en el camino
    - Acorta la distancia suavemente (lerp asimétrico: acerca rápido, aleja lento)
    - Clampea a 0 para no confundirse con zonas de agua

### JUGADOR
32. Físicas — gravedad 9.8, velocidadY acumulativa, colisión con terreno
33. Salto con espacio (solo en suelo), fuerza 5.0
34. Movimiento WASD relativo al ángulo de cámara
35. Correr con Shift — velocidad doble (5 → 10 u/s)
36. Sistema de stamina completo:
    - Stamina 0–100, consumo 12/s corriendo, recarga 10/s caminando
    - Agotamiento: al llegar a 0 → flag agotado, velocidad cae a 2.5 u/s
    - Penalización mínima de 2.5s aunque recargue rápido
    - Recarga al 50% de velocidad mientras agotado
    - Umbral de recovery: 25 stamina + tiempo mínimo cumplido
37. Sistema de ataque:
    - Click izquierdo → golpe en cono frontal de 90°, alcance 2.5 unidades
    - Cooldown 0.5s entre golpes
    - Daño 34 por golpe (3 golpes = muerte de zombie)
    - Flash blanco del jugador durante 0.15s al atacar
38. Penalización de velocidad por peso del inventario:
    - 0–50% del peso max → sin penalización
    - 50–100% → hasta -40% de velocidad

### ZOMBIES
39. Cubo rojo, físicas completas con gravedad igual que el jugador
40. Máquina de estados con 3 estados:
    - IDLE (rojo oscuro): deambula aleatoriamente, cambia dirección cada 2–4s
    - PERSIGUIENDO (rojo vivo): detecta jugador a 20u, se mueve directo
    - ATACANDO (naranja): a 1.5u, aplica 8 daño/segundo al jugador
41. Knockback al recibir daño:
    - Vuela en la dirección del golpe con fuerza 6.0
    - Duración 0.2s, frenado gradual
    - Flash blanco-naranja durante el knockback
    - No hace IA ni daño mientras tiene knockback activo
42. Spawn con altura +3u sobre el terreno (evita enterrarse)
    - Primera oleada a los 0.5s de inicio (espera a que carguen los chunks)
    - 5 a 10 zombies por oleada, radio 15–40u del jugador
43. Respawn del jugador a los 3s de morir, nueva oleada
44. Partículas de muerte: 12 cubitos rojos/marrones con gravedad al morir

### HUD
45. Tres barras en esquina inferior izquierda:
    - Salud: verde → amarillo → rojo según porcentaje
    - Stamina: azul → amarillo → naranja, parpadeo rojo/naranja si agotado
    - Peso: marrón claro → naranja → rojo según carga del inventario
46. Barras de vida flotantes sobre zombies dañados:
    - Proyección 3D→2D con matrices view/projection
    - Verde si >50% vida, rojo si ≤50%
    - Solo visible cuando el zombie recibió daño (pct < 1.0)
47. Panel de inventario centrado en pantalla (Tab para abrir/cerrar)
    - Grilla 6x5 = 30 slots
    - Slots con color según tipo de item
    - Borde amarillo en slot seleccionado
    - Barra de cantidad para items apilables

### INVENTARIO Y LOOT
48. Sistema de inventario completo:
    - Grilla 6 columnas × 5 filas = 30 slots
    - Peso máximo: 30 kg
    - Apilado automático — busca slot del mismo tipo antes de abrir uno nuevo
49. Items implementados:
    - Venda: peso 0.2kg, stack hasta 5, cura 25% de salud
    - Kit Médico: peso 0.8kg, stack hasta 3, cura 60% de salud
50. Loot al matar zombies:
    - 70% chance de venda
    - 20% chance de kit médico
51. Controles del inventario:
    - Tab: abrir/cerrar (bloquea movimiento y ataque mientras abierto)
    - Flechas: navegar entre slots
    - F: usar item seleccionado (aplica curación instantánea)

### IDENTIDAD VISUAL
52. Ícono del .exe — "Hollow Earth": esfera abierta mostrando interior oscuro con brillo verde
    - Estilo pixel art + painted, generado por Gemini AI
    - Convertido a .ico con múltiples tamaños (16, 32, 48, 64, 128, 256px)
    - Integrado via recursos.rc en CMakeLists.txt

---

## RENDIMIENTO ACTUAL
```
Hardware de desarrollo:
  CPU → Dual core 3GHz (Pentium G3260)
  RAM → 8GB DDR3
  GPU → GeForce GT 610 (1GB VRAM, OpenGL 3.3, driver 353.54)
  OS  → Windows 10 64-bit

FPS en juego:
  Sin zombies, quieto     → ~170 FPS
  Moviéndose              → ~150 FPS
  Con 10 zombies activos  → ~120–140 FPS
  Con agua + partículas   → ~100–120 FPS
```

---

## BUGS CONOCIDOS Y RESUELTOS

### Resueltos ✅
- Agua.png original crasheaba el juego (driver viejo, ICC profile en PNG) → convertir con Pillow a RGB limpio
- `Agua::Agua()` llamaba a OpenGL antes de GLAD → constructor vacío + `inicializar()` explícito
- Zombies invisibles al morir → `limpiarMuertos()` llamado inmediatamente después del ataque
- Zombies flotantes al spawnear → spawn con +3u sobre terreno, espera 0.5s inicial
- Crash al matar el último zombie → erase inmediato post-ataque + guard en dibujarBarrasZombies
- Textura de agua crasheaba → conversión de PNG con metadata ICC a RGB limpio

### Pendientes / Conocidos ⚠️
- Cámara no siempre se acerca perfectamente en montañas muy empinadas
- Algunos zombies pueden quedar levemente elevados en pendientes pronunciadas

---

## PRÓXIMO PASO 🔄
- Más tipos de items (comida, materiales, armas)
- Texto en HUD (librería de fuentes — stb_truetype o similar)
- Sistema de día/noche con cambio de iluminación
- Oleadas automáticas cuando se matan todos los zombies

---

## PENDIENTE EN ENGINE ❌
- Hotbar visible con items equipados
- Texto renderizado en HUD (nombres de items, cantidades)
- Construcción básica (colocar bloques)
- Carga de modelos .gltf (reemplazar cubos)
- Audio con OpenAL
- Sistema de mods con Lua + sol2
- IA avanzada con Python + pybind11
- Bullet Physics
- Múltiples biomas
- Sistema de día/noche
- Guardado de partida (.hesave)

---

## DECISIONES TÉCNICAS TOMADAS

| Decisión | Elección | Razón |
|----------|----------|-------|
| Renderer | SDL3 + OpenGL 3.3 | Liviano, compatible con hardware mínimo |
| Loader OpenGL | GLAD | Más moderno que GLEW, sin DLL extra |
| Carga de imágenes | SDL3_image | Integrado con SDL3, soporta PNG/BMP/JPG |
| Audio | OpenAL (futuro) | Open source, coherente con GPL |
| Scripting mods | Lua + sol2 (futuro) | Más conocido, liviano |
| IA avanzada | Python + pybind11 (futuro) | Mejores librerías de ML |
| Física | Gravedad propia (ahora) / Bullet Physics (futuro) | Simple primero |
| Ruido | FastNoiseLite (ahora) / FastNoise2 (futuro) | Simple primero |
| Ciudades | WFC + Prefabs (futuro) | Híbrido calidad/infinito |
| Licencia | GPL v3 | Libertad con reciprocidad |
| Idioma código | Español | Equipo hispanohablante |
| Extensiones | .he* propias (futuro) | Control total del formato |
| Build | CMake + MinGW | Estándar multiplataforma |
| Ícono | .ico via recursos.rc | Estándar Windows |

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

### 21/03/2026 — Sesión 3, 4 y 5
- Sistema de chunks dinámicos (carga/descarga por distancia)
- Shaders separados en archivos .glsl
- Iluminación difusa + ambiental con normales por vértice
- Niebla exponencial
- Texturas con SDL3_image (PNG)
- Salto del jugador
- Carrera con Shift + sistema de stamina completo con agotamiento
- HUD con barras de salud, stamina y peso
- Árboles procedurales por chunk (cubos tronco/copa)
- Agua semitransparente animada con textura
- Skybox con cubemap de 6 caras BMP
- Colisión de cámara con terreno (barrido horizontal)
- Multitextura por altura en fragment shader (arena/pasto/roca/nieve)
- Sombras suaves con smoothstep
- Zombies con IA (IDLE/PERSIGUIENDO/ATACANDO), gravedad, knockback
- Partículas de muerte (cubitos con gravedad)
- Sistema de combate: click izquierdo, cono frontal 90°, cooldown 0.5s
- Barras de vida flotantes sobre zombies
- Respawn del jugador a los 3s
- Ícono del .exe (Hollow Earth, pixel art + painted)
- Inventario 6x5 = 30 slots con peso (30kg máx)
- Items: Venda (cura 25%) y Kit Médico (cura 60%)
- Loot al matar zombies (70% venda, 20% kit médico)
- Tab abre inventario, flechas navegan, F usa item

---

## LÍNEAS DE CÓDIGO ACTUALES
```
~2.350 líneas (sin contar inventario.cpp/h recién agregados)
Con inventario estimado: ~2.600 líneas
```

---

## CÓMO USAR ESTE ARCHIVO

Antes de cada sesión con Claude, pasá ambos archivos:
- `HOLLOW_EARTH_DESIGN.md` → contexto de diseño
- `HOLLOW_EARTH_PROGRESS.md` → estado actual

Claude los lee y retoma desde donde dejaste sin perder contexto.
