# Hollow Earth — Documento de Diseño Completo
> Survival zombie open source, GPL v3  
> Desarrollado por: [Tu nombre] + Claude (Anthropic)

---

## CONCEPTO GENERAL

Hollow Earth es un juego de supervivencia zombie en mundo abierto 3D isométrico.
Inspirado en Project Zomboid, Unturned y Minecraft, pero con sistemas más profundos,
mejor optimización y código completamente abierto.

**Ambientación:** Contemporáneo 2020s  
**Perspectiva:** Isométrica 3D, cámara orbital  
**Licencia:** GPL v3 — código fuente público desde el día uno  

---

## LORE

Un virus de origen antiguo — millones de años — estaba atrapado en el permafrost
de Siberia o en cuevas subterráneas profundas. El calentamiento global derritió el
glaciar. Una expedición científica lo encontró sin saber qué era. Lo estudiaron, lo
trasladaron, y escapó del laboratorio.

El virus es tan antiguo que el sistema inmune humano nunca lo vio — sin defensa posible.

**Coherencia con el nombre:** Algo vino de adentro de la tierra.  
**Narrativa ambiental:** El lore se descubre jugando — notas, libros, radios, grabaciones.  
**Nadie sabe exactamente qué pasó** — el misterio es parte del juego.

---

## STACK TECNOLÓGICO

```
Engine:          C++
Rendering:       SDL2 + OpenGL
Física:          Bullet Physics
Audio:           OpenAL
Ruido:           FastNoise2
Mods:            Lua + sol2 (puente C++/Lua)
IA:              Python + pybind11 (puente C++/Python)
Compilación:     CMake
Versioning:      Git + GitHub
IDE:             VS Code + MinGW
Licencia:        GPL v3
```

---

## EXTENSIONES DE ARCHIVO PROPIAS

```
.hemap      → mapas del mundo
.hesave     → partidas guardadas (binario comprimido)
.hemod      → paquetes de mods (ZIP renombrado)
.heprefab   → prefabs del editor (edificios, estructuras)
.heconfig   → configuración del juego
```

**Estructura de un .hemod:**
```
mi_mod.hemod
├── mod.info       → nombre, versión, autor
├── scripts/       → archivos Lua
├── assets/        → texturas, sonidos
├── prefabs/       → edificios en .heprefab
└── python/        → módulos de IA opcional
```

---

## GENERACIÓN PROCEDURAL DEL MUNDO

**Sistema de ruidos:**
```
Simplex Noise + FBM   → terreno base (superficie)
Ruido 3D Simplex      → cuevas subterráneas
Voronoi + temperatura → sistema de biomas
WFC                   → ciudades y estructuras
Prefabs comunitarios  → edificios diseñados a mano
```

**Sistema de biomas:**
```
Temperatura + humedad definen el bioma
Transiciones suaves entre biomas
Cada bioma tiene flora, fauna zombie y clima propio
```

**Desastres naturales por bioma:**
```
Bioma costero     → tsunamis, huracanes
Bioma montañoso   → avalanchas, terremotos
Bioma llanura     → tornados, tormentas eléctricas
Zonas volcánicas  → erupciones
Cualquier bioma   → lluvia ácida, niebla tóxica
```

**Frecuencia:** Según coordenadas, altura y bioma del mapa.  
**Procesamiento:** Offscreen durante el sueño del jugador.

---

## SISTEMA DE CHUNKS Y MUNDO

```
Chunks dinámicos    → carga y descarga según posición del jugador
Tiles 3D            → base del mundo
Mundo infinito      → generación bajo demanda
Editor de mapas     → herramienta separada, también GPL
Prefabs             → creados por comunidad, colocados proceduralmente
```

---

## CICLO DÍA/NOCHE Y TIEMPO

```
Duración día/noche  → ~20 minutos (configurable)
Estaciones          → 4 estaciones como PZ
Clima dinámico      → lluvia, nieve, niebla, tormenta
Tiempo configurable → inicio Día 1 hasta 10+ años después
```

---

## SISTEMA DE SUEÑO Y SIMULACIÓN OFFSCREEN

**Cuando el jugador duerme:**
```
→ Pantalla se oscurece (como PZ)
→ Tiempo se acelera
→ Rendering casi apagado
→ TODO el CPU va a simulación offscreen

El juego procesa:
→ Clima y desastres en coordenadas del mapa
→ Conflictos entre facciones
→ Deterioro de estructuras
→ Crecimiento o colapso de grupos
→ Eventos aleatorios (tirada de dados)
→ Impacto del clima en grupos y terreno
```

**Fragmentos durante el sueño:**
```
Imágenes estáticas con texto — pistas de lo que pasó
"Escuchás disparos a lo lejos"
"El olor a humo te despierta por un momento"
Crea tensión sin costo de rendering
```

**Al despertar:**
```
Mundo actualizado según simulación
Al acercarse a zona → generador lee estado y construye acorde
```

---

## SISTEMA DE INFECTADOS

**Comportamiento base — memoria residual:**
```
Cada zombie conserva habilidades de su vida anterior
No es inteligencia — es memoria muscular e instinto
Un cocinero agarra objetos cortantes
Un policía puede usar su arma
Un niño es pequeño, rápido, difícil de ver
```

**Arquetipos:**
```
COMÚN      → lento, débil, peligroso en manada
CORREDOR   → ex deportista, sprinter
TANQUE     → ex gimnasio, absorbe daño, golpe fuerte
TREPADOR   → escala edificios, ataca desde arriba
BOXEADOR   → esquiva, contraataca, bloquea golpes
NADADOR    → peligroso en zonas de agua
MILITAR    → coordinado, usa cobertura instintivamente
NIÑO       → rapidísimo, difícil de golpear, en grupos
```

**Comportamiento adicional:**
```
Van a lugares que frecuentaban
Algunos activos de noche, otros de día
Algunos "trabajan" — barren, acomodan cosas
Reaccionan a estímulos conocidos — música, olores
```

---

## SISTEMA DE COMBATE

**Referencias:**
- Brawlhalla — combate fluido con momentum
- Epic Fight / Better Combat (Minecraft) — animaciones direccionales

**Características:**
```
Combate rápido y variado
Stamina ligada al combate — no se puede spammear
Esquiva y contrataque como mecánica core
Diferentes armas cambian completamente el estilo
```

---

## SISTEMA DE ARMAS

**Ramas separadas (no todo junto como PZ):**

```
ESPADAS
→ Una mano (gladius, sable)
→ Dos manos (mandoble, katana)
→ Habilidad única: parada y contraataque

HACHAS
→ Una mano (hacha de mano)
→ Dos manos (hacha de guerra, leñador)
→ Habilidad única: romper armadura y puertas

CONTUNDENTES
→ Una mano (bate, martillo)
→ Dos manos (mazo, palo de béisbol)
→ Habilidad única: aturdir y derribar

LANZAS Y ARMAS DE ASTA
→ Alcance largo, mantiene distancia
→ Habilidad única: detener cargas de corredores

CUCHILLOS Y ARMAS CORTAS
→ Rápidos, silenciosos
→ Habilidad única: sigilo, ejecuciones silenciosas

IMPROVISADAS
→ Patas de cabra, tubos, sillas
→ Se rompen pero están en todas partes
→ Cualquier profesión las usa bien

ARCOS Y BALLESTAS
→ Silenciosas, munición recuperable
→ Habilidad única: disparo de precisión, punto débil

ARMAS DE FUEGO
→ Pistolas, rifles, escopetas
→ Escasean con el tiempo
→ Habilidad única: disparo rápido, mantenimiento
```

**Progresión dentro de cada rama:**
```
Nivel 1-3   → torpe, lento, se cansa rápido
Nivel 4-6   → competente, combos básicos
Nivel 7-9   → fluido, combos avanzados
Nivel 10    → maestría — habilidad única desbloqueada
```

**Curva de escasez de armas de fuego:**
```
Inicio    → armas de fuego disponibles
Medio     → mezcla, hay que elegir cuándo disparar
Tardío    → armas blancas dominan, las de fuego son oro
```

---

## ARTESANÍA DE ARMAS DE FUEGO

```
NIVEL 1 — Zip gun
→ Un disparo, puede romperse al usarla
→ Materiales: tubo de metal, resorte, clavo

NIVEL 2 — Revólver artesanal
→ 3-5 disparos, más confiable
→ Requiere herramientas básicas

NIVEL 3 — Pistola artesanal
→ Semi automática rudimentaria
→ Se encasquilla más que una real

NIVEL 4 — Escopeta artesanal
→ Recortada, potente a corta distancia
→ La más viable en apocalipsis
```

**Munición:**
```
Comercial   → confiable, daño estándar
Recargada   → aceptable, ligera reducción de daño
Artesanal   → impredecible, puede fallar, daño variable
```

---

## SISTEMA DE PROFESIONES Y TRAITS

**El problema de PZ que evitamos:**
A largo plazo todos los personajes terminan igual. En Hollow Earth las profesiones
dan acceso a habilidades ÚNICAS que otros no pueden aprender, o las aprenden
muchísimo más lento.

**Ejemplo:**
```
MÉDICO
→ Cirugía de campo — otros NUNCA pueden hacerla
→ Diagnóstico de enfermedades
→ Farmacología — crea medicamentos

MILITAR
→ Tiro de precisión bajo estrés
→ Coordinación táctica — buff a aliados
→ Desactivar trampas

ATLETA
→ Resistencia superior permanente
→ Parkour — escala donde otros no pueden
→ Recuperación física más rápida
```

**Traits con peso real:**
```
HEMOFÍLICO (negativo)
→ Cualquier herida sangra el doble
→ Las vendas duran la mitad
→ Más puntos para compensar

SANGRE FRÍA (positivo)
→ Zombies con memoria residual no te detectan fácil
→ Podés pasar cerca sin que reaccionen
```

---

## SISTEMA DE APTITUDES FINALES

Al terminar de elegir profesión y traits, el juego ofrece **3 opciones** basadas
en las elecciones — aptitudes naturales que el personaje descubre sobre sí mismo.

**Ejemplo:**
```
Elegiste MILITAR + ATLETA + SANGRE FRÍA
El juego ofrece:

OPCIÓN 1: OPERATIVO ESPECIAL
→ Sigilo + armas de fuego + resistencia superior

OPCIÓN 2: COMBATIENTE CUERPO A CUERPO
→ Armas blancas + defensa + intimidación zombie

OPCIÓN 3: SUPERVIVIENTE TÁCTICO
→ Trampas + liderazgo + conocimiento del terreno
```

---

## ESPECIALISTA VS COMODÍN

```
ESPECIALISTA
→ Una rama al máximo muy rápido
→ Las otras suben muy lento
→ En su arma es imbatible, fuera de ella vulnerable

COMODÍN
→ Todas las ramas a velocidad normal
→ Nunca llega al máximo de ninguna
→ Adaptable, nunca el mejor pero nunca perdido
```

Con suficiente tiempo un personaje puede aprender más de una rama,
pero será muy lento. La especialización es la ruta eficiente.

---

## SISTEMA DE SALUD FÍSICA

```
Condición física como stat dinámico
Sin buena alimentación → pérdida de masa muscular → menos daño
Con ejercicio y buena dieta → mejora de stats físicos
Peso corporal afecta velocidad y stamina
Fracturas, esguinces, desgarros musculares
Rehabilitación progresiva — no sanás de golpe
Enfermedades reales — no solo el Knox de PZ
```

---

## MODOS DE INICIO

```
MODO INICIO       → Día 1, sociedad colapsando, NPCs abundantes
MODO SUPERVIVIENTE → Semanas después, como PZ
MODO VETERANO     → Meses después, mundo hostil y escaso
MODO PERSONALIZADO → Hasta 10+ años después
```

Cada modo cambia cantidad de NPCs, recursos, estado de facciones y dificultad.

---

## SISTEMA DE NPCs Y FACCIONES

**IA por capas:**
```
CAPA 1 — Comportamiento base (C++)
→ Necesidades básicas, miedos, objetivos
→ Muy liviano

CAPA 2 — Toma de decisiones (Lua)
→ Reglas complejas basadas en memoria

CAPA 3 — Aprendizaje (Python)
→ Corre cada X minutos de juego, no en tiempo real
→ Procesa en background
```

**Cada NPC tiene:**
```
→ Valentía
→ Lealtad
→ Inteligencia
→ Memoria — cuánto recuerda y por cuánto tiempo
→ Profesión y habilidades de vida anterior
```

**Sistema de facciones dinámicas:**
```
No son fijas — se forman, crecen, colapsan
Cada facción tiene ideología, recursos, memoria colectiva
Aprenden de encuentros con el jugador
Comparten información entre miembros
Los rumores viajan y se distorsionan
```

**Sistema de rumores:**
```
Mataste zombies en un pueblo
Un NPC lo vio → lo cuenta
Viaja distorsionado → "hay un grupo armado en el norte"
Otra facción manda exploradores a verificar
```

**Simulación offscreen de facciones:**
```
Cada grupo tiene stats internos:
→ Fuerza militar (0-100)
→ Recursos (0-100)
→ Moral (0-100)
→ Organización (0-100)

Cada tick (durante sueño del jugador):
→ Conflictos según cercanía y stats
→ Eventos aleatorios — tirada de dados
→ Crecimiento o colapso
→ Impacto del clima en recursos
```

---

## SISTEMA DE FACCIONES DEL JUGADOR

```
El jugador puede crear su propia facción
→ Nombre personalizado
→ Logo: base geométrica + símbolo + colores + texto
→ Reclutar NPCs
→ Establecer territorio
→ Comerciar o guerrear con otras facciones
```

---

## DETERIORO DEL MUNDO

```
DÍA 1        → ciudades intactas, ejército presente
1 MES        → zonas de guerra, grupos organizados
6 MESES      → ciudades controladas por facciones
1 AÑO        → guerras de recursos, comercio rudimentario
5 AÑOS       → civilización nueva emergiendo
10 AÑOS      → nuevo orden, ciudades en ruinas exploradas
```

**Estados de estructuras:**
```
ESTADO 1 → Intacto
ESTADO 2 → Dañado (ventanas rotas, puertas forzadas)
ESTADO 3 → Deteriorado (paredes dañadas, techos caídos)
ESTADO 4 → En ruinas (solo estructura básica)
ESTADO 5 → Colapso total (escombros)
```

---

## SISTEMA DE MODS

```
Lua + sol2     → items, mecánicas, eventos, facciones
Python         → módulos de IA custom
API abierta    → acceso total
Código fuente  → GPL desde el día uno
```

**Lo que los modders pueden hacer:**
```
→ Nuevas armas, items, vehículos
→ Nuevos biomas y generación de terreno
→ Facciones completas con ideología y diálogos
→ Sistemas de IA custom con Python
→ Modelos de ML propios (hardware del usuario permitting)
→ Nuevos arquetipos de infectados
→ Prefabs de edificios y ciudades
```

---

## DISTRIBUCIÓN

```
Licencia      → GPL v3
Plataforma    → Windows primero, Linux/Mac por comunidad
Precio        → Gratuito y open source
Monetización  → Donaciones / Patreon opcional
Repositorio   → GitHub público
```

---

## HARDWARE OBJETIVO MÍNIMO

Basado en el hardware de desarrollo:
```
CPU → Dual core 3GHz (Pentium G3260 o equivalente)
RAM → 8GB DDR3
GPU → 1GB VRAM (GeForce GT 610 o equivalente)
OS  → Windows 10 64-bit
```

**Filosofía:** Si corre en el hardware de desarrollo, corre en todo.

