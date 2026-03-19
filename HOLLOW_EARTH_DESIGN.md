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


---

## SISTEMA DE CONSTRUCCIÓN

**Filosofía:** Libertad total pero con física real. Referencia: Valheim.

```
REGLAS BASE:
→ Toda estructura necesita soporte
→ No puede flotar — si se quita el soporte colapsa
→ Los materiales tienen resistencia diferente
```

**Materiales (menor a mayor resistencia):**
```
Madera improvisada  → fácil de conseguir, se pudre con lluvia
Madera trabajada    → más resistente, requiere habilidad
Ladrillos           → muy resistente, requiere horno y arcilla
Metal               → el más resistente, requiere taller completo
Hormigón            → máxima resistencia, raro en apocalipsis tardío
```

**Elementos construibles:**
```
→ Paredes (enteras, con ventana, con puerta)
→ Pisos y techos
→ Escaleras y rampas
→ Vigas de soporte
→ Trampas integradas
→ Torretas improvisadas
→ Barricadas rápidas (sin soporte necesario)
```

**Deterioro:**
```
Madera sin mantenimiento → se pudre con lluvia y tiempo
Metal sin tratamiento    → se oxida, pierde resistencia
Zombies tanque pueden derribar paredes débiles
Desastres naturales dañan estructuras según material
```

---

## VEHÍCULOS

```
Bicicleta         → silenciosa, sin combustible, lenta
Moto              → rápida, poco espacio, consume poco
Auto sedán        → equilibrado, 4 pasajeros
Pickup/camioneta  → carga alta, todo terreno básico
SUV 4x4           → todo terreno real, resistente
Furgoneta         → máxima carga, lenta
Ambulancia        → kit médico integrado
Patrullero        → reforzado, equipo policial
Vehículo militar  → blindado, muy resistente
Tractor           → para agricultura, lentísimo
Bote              → zonas de agua y ríos
```

**Mantenimiento:**
```
Motor, frenos, suspensión, combustible
Sin mantenimiento → falla gradualmente
Reparable con habilidad de mecánica
Piezas escasean con el tiempo
```

---

## SISTEMA DE AGRICULTURA Y COMIDA

**Tipos de plantas:**
```
VERDURAS
→ Tomate, papa, zanahoria, maíz, lechuga
→ Cada una con temporada óptima

FRUTAS
→ Manzana, naranja, frutilla, uva
→ Frutales tardan más pero producen más
→ Algunos necesitan polinización

GRANOS
→ Trigo, arroz, avena
→ Más espacio, procesables en harina

PLANTAS MEDICINALES
→ Aloe vera, menta, lavanda, equinácea
→ Ingredientes para medicamentos artesanales
→ El médico las aprovecha mejor

PLANTAS TÓXICAS
→ Se parecen a las comestibles
→ Envenenamiento si se comen por error
→ Botánica permite identificarlas fácil

HONGOS
→ Comestibles y venenosos mezclados
→ Algunos medicinales
→ Crecen en zonas húmedas y oscuras
```

**Ciclo de la planta:**
```
Semilla → brote → planta joven → madura → cosecha → semillas nuevas

Variables:
→ Estación del año
→ Temperatura del bioma
→ Humedad (lluvia o riego)
→ Calidad del suelo
→ Luz solar
→ Plagas
```

**Calidad del suelo:**
```
Suelo virgen      → bueno pero se agota
Suelo agotado     → mal rendimiento
Suelo abonado     → mejor rendimiento
Compost           → fabricado con desechos orgánicos
Tierra contaminada → plantas tóxicas aunque sean comestibles
```

**Enfermedades de plantas:**
```
Hongos            → se contagia a plantas cercanas
Plagas de insectos → reducen cosecha
Sequía            → sin agua las plantas mueren
Helada            → mata plantas fuera de temporada
Suelo contaminado → altera plantas normalmente comestibles
```

**Procesamiento:**
```
Cruda      → come pero menos nutrición, riesgo de enfermedad
Cocida     → mejor nutrición, elimina bacterias
Conservada → dura más (ahumado, salado, enlatado artesanal)
Podrida    → envenenamiento, atrae animales
```

**Sistema de nutrición:**
```
Carbohidratos → energía para moverse
Proteínas     → recuperación física y muscular
Grasas        → resistencia al frío
Vitaminas     → sistema inmune
Agua          → deshidratación es letal

Deficiencias:
Sin proteínas  → pérdida de fuerza y masa muscular
Sin vitaminas  → sistema inmune débil
Sin grasas     → el frío afecta más
Deshidratación → stamina cae drásticamente
```

---

## SISTEMA DE DEFENSA DE BASE

**Defensas pasivas (construcción):**
```
→ Paredes de diferentes materiales
→ Fosas — zombies caen y quedan atrapados
→ Alambres de púas — ralentizan y dañan
→ Estacas — daño al atravesarlas
→ Torres de vigilancia
```

**Trampas activas:**
```
→ Trampas de presión — explotan o disparan
→ Trampas de fuego
→ Trampas de sonido — atraen zombies a zona específica
→ Trampas de red — inmovilizan
```

**Defensas activas (requieren energía):**
```
→ Torretas improvisadas — necesitan munición
→ Sistemas de alarma
→ Generadores — dan energía a defensas eléctricas
→ Reflectores — iluminan de noche
```

**Gestión:**
```
→ Requiere mantenimiento constante
→ Se deterioran con el tiempo
→ Zombies tanque rompen paredes débiles
→ Desastres naturales dañan estructuras
→ NPCs de tu facción defienden mientras dormís
```

---

## COMERCIO Y ECONOMÍA

**Sistema de trueque:**
```
No hay moneda — todo es trueque directo
El valor de los items cambia según:
→ Escasez en la zona
→ Época del apocalipsis
→ Facción con la que comerciás

Agua potable     → muy valiosa siempre
Medicamentos     → aumentan valor con el tiempo
Munición         → oro del apocalipsis tardío
Comida           → valor varía según estación
Herramientas     → siempre demandadas
Combustible      → escasea rápido
```

**Relaciones comerciales:**
```
Mejor reputación con una facción → mejores precios
Enemistado → no comercian o precios abusivos
Neutral → precios estándar
Aliado → pueden compartir recursos en emergencias
```


---

## SISTEMA DE PROGRESIÓN DE HABILIDADES

**Filosofía — Prueba y error (Opción B):**
```
Sin niveles visibles al jugador
Las habilidades suben haciendo — igual que la vida real
El juego acumula experiencia internamente
Cada X usos (aleatorio, no fijo) → boost invisible
Lo notás porque algo se siente diferente
"Che, estoy cortando más rápido..."
```

**Tres vías de aprendizaje:**
```
PRUEBA Y ERROR    → más lento, siempre disponible
AUTODIDACTA       → libros acortan el rango aleatorio entre boosts
                    El aumento sigue siendo el mismo — llegás antes
MAESTRO NPC       → más rápido de los tres
                    El más difícil — necesita encontrar al NPC,
                    ganarte su confianza y que sobreviva
```

**El techo máximo depende de profesión y aptitud final.**
Un comodín nunca llega al máximo de ninguna rama.
Un especialista llega al máximo solo en su rama, muy lento en las demás.

---

## SISTEMA DE ENFERMEDADES

**Lo que influye en el sistema inmune:**
```
POSITIVO:
→ Alimentación balanceada con todas las vitaminas
→ Ejercicio regular
→ Dormir bien — en cama, lugar seco, horas suficientes
→ Vivir en lugar seco y ventilado
→ Higiene — lavarse con agua limpia
→ Medicamentos preventivos
→ Vacunas si las encontrás

NEGATIVO:
→ Malnutrición o deficiencias vitamínicas
→ Sin dormir o dormir mal
→ Vivir en lugar húmedo, sucio o con cadáveres cerca
→ Heridas sin tratar
→ Estrés extremo — combate constante sin descanso
→ Agua contaminada
→ Comida cruda o podrida
→ Temperatura extrema sin ropa adecuada
```

**Tipos de enfermedades:**
```
BACTERIANAS:
→ Salmonela — comida cruda o mal conservada
→ Cólera — agua contaminada
→ Tétanos — heridas con objetos oxidados
→ Infección de herida — heridas sin limpiar
→ Neumonía — frío extremo + sistema inmune bajo

VIRALES:
→ Gripe común — contacto con infectados vivos
→ Resfriado — frío + sistema inmune bajo
→ El virus de Hollow Earth — mordida de zombie

PARASITARIAS:
→ Giardia — agua de fuente no purificada
→ Parásitos intestinales — carne cruda de animales
→ Sarna — contacto con animales o personas infectadas

AMBIENTALES:
→ Intoxicación — plantas tóxicas, hongos venenosos
→ Envenenamiento por radiación — zonas contaminadas
→ Golpe de calor — biomas calientes sin hidratación
→ Hipotermia — frío extremo sin ropa adecuada
→ Deshidratación severa
→ Intoxicación por humo — incendios mal ventilados
```

**Progresión:**
```
ETAPA 1 — Síntomas leves (el jugador puede ignorarlos)
→ Ligero cansancio extra, leve reducción de stamina
→ El juego no avisa explícitamente

ETAPA 2 — Síntomas moderados
→ Reducción notable de stats
→ Fiebre — visión ligeramente alterada

ETAPA 3 — Síntomas graves
→ Stats muy reducidos, visión borrosa, mareos

ETAPA 4 — Crítico
→ El personaje apenas puede moverse
→ Sin tratamiento inmediato → muerte
```

**Tratamiento específico por enfermedad:**
```
Infección bacteriana → antibióticos o plantas medicinales
Parásitos           → antiparasitarios o ajo + plantas
Hipotermia          → calor, ropa seca, líquidos calientes
Golpe de calor      → sombra, agua, bajar temperatura
Intoxicación        → carbón activado, vomitar, tiempo
Tétanos             → vacuna preventiva o antibióticos potentes
Neumonía            → descanso, calor, antibióticos
```

**El médico:**
```
→ Diagnóstico preciso en Etapa 1
→ Tratamientos más efectivos con menos recursos
→ Fabrica medicamentos con plantas
→ Conoce plantas medicinales automáticamente
```

---

## SISTEMA DE ELECTRICIDAD

**Fuentes de energía:**
```
COMBUSTIBLE:
→ Generador a nafta — potente, hace ruido, atrae zombies
→ Generador a diesel — más eficiente, mismo problema
→ Generador a biogás — fabricable con desechos, silencioso

RENOVABLES:
→ Panel solar — silencioso, depende del clima y bioma
→ Molino de viento — depende del bioma y altura
→ Turbina de agua — necesita río o corriente, muy eficiente
→ Pila de combustible artesanal — compleja, estable

ALMACENAMIENTO:
→ Baterías — guardan energía para noche o días sin viento/sol
→ Sin almacenamiento la energía se desperdicia
```

**Consumo:**
```
Bajo:    luces LED, cargadores, radios
Medio:   refrigerador, bomba de agua, herramientas eléctricas
Alto:    purificador UV, taller eléctrico, torretas
```

**Sistema de circuitos:**
```
Cables entre fuente → almacenamiento → consumo
Si un cable se corta → falla el circuito
Los zombies pueden dañar cables externos
Los desastres naturales también
Sin electricidad → todo funciona pero más lento
Siempre hay alternativas manuales
```

---

## SISTEMA DE AGUA

**Calidad por fuente:**
```
EXCELENTE → agua de montaña, manantial alto, lluvia en zona limpia
BUENA     → pozo profundo, río de montaña sin contaminación
REGULAR   → río de llanura, pozo superficial, lluvia sin filtrar
MALA      → lago estancado, río cerca de ciudad o industria
PELIGROSA → zona con cadáveres, industrial, desastre químico
```

**Purificación — menor a mayor efectividad:**
```
NIVEL 1 — Hervir
→ Elimina bacterias y virus, no químicos ni metales

NIVEL 2 — Filtro artesanal (arena + carbón + grava)
→ Mejora calidad, no purifica completamente

NIVEL 3 — Pastillas purificadoras
→ Elimina bacterias y parásitos, no químicos

NIVEL 4 — Filtro cerámico
→ Elimina casi todo lo biológico, fabricable con alfarería

NIVEL 5 — Purificador UV eléctrico
→ Elimina todo lo biológico, necesita electricidad

NIVEL 6 — Destilación
→ Elimina absolutamente todo, lento y consume combustible
```

**Sistema de cañerías:**
```
Conectan fuente → tanque → puntos de uso
Sin bomba → tanque más alto que el punto de uso
Con bomba eléctrica → podés bombear cuesta arriba
Las cañerías se pueden romper — requieren mantenimiento
```

**Bombas:**
```
Manual    → funciona siempre, requiere esfuerzo físico
E�lica    → molino conectado a pozo, sin electricidad
Eléctrica → la más eficiente, depende de electricidad
A vapor   → fabricable, consume madera o carbón
```

**Contaminación:**
```
Cadáver en río upstream → contamina toda el agua del río
Zona industrial         → contamina agua subterránea
Lluvia ácida            → contamina fuentes superficiales temporalmente
Explosiones/incendios   → contaminación temporal del área
```

---

## FAUNA

**Animales salvajes:**
```
MAMÍFEROS:
→ Ciervo — carne, cuero, huesos
→ Jabalí — agresivo, carne y cuero
→ Conejo — pequeño, rápido, carne
→ Oso — muy peligroso, carne, grasa, piel
→ Lobo — en manada, territorial, piel
→ Zorro — esquivo, piel fina
→ Rata — en ciudades, portadora de enfermedades
→ Murciélago — en cuevas, portador de virus

AVES:
→ Gallina salvaje — huevos y carne
→ Pato — zonas de agua
→ Cuervo — indicador de cadáveres cercanos
→ Águila — difícil de cazar, plumas para flechas

PECES:
→ Trucha — agua limpia de montaña
→ Carpa — agua estancada, menos nutritiva
→ Bagre — ríos, fácil de pescar
→ Calidad del pez depende de calidad del agua

INSECTOS:
→ Abejas — miel, polinización de cultivos
→ Lombrices — mejoran suelo, cebo de pesca
→ Moscas — indicador de cadáveres o comida podrida
→ Mosquitos — zonas húmedas, pueden transmitir enfermedades
```

**Comportamiento:**
```
Cada animal tiene rutinas y horarios
Los predadores huyen del ruido pero vuelven
Los herbívoros huyen de zombies y jugadores
Los animales detectan zombies antes que el jugador
Comportamiento cambia según estación
```

**Caza:**
```
Arco y ballesta → silenciosa, ideal
Armas de fuego  → efectiva pero hace ruido
Trampas         → pasiva, revisás periódicamente
Lanza           → requiere acercarse, silenciosa

Habilidad de caza:
→ Detectar rastros
→ Moverse sin espantar
→ Identificar zonas de caza
→ Aprovechar mejor la carne al despiezar
```

**Domesticación:**
```
GALLINAS  → capturás pollos jóvenes, alimentás, ponen huevos
CERDOS    → atraés con comida, carne renovable, comen desechos
PERROS    → los más útiles, alertan zombies, rastrean, defienden
           descuidados se van
CABALLOS  → transporte sin combustible, más silencioso que auto
           requiere silla, herraje, espacio y pasto
```

**Animales y zombies:**
```
Los zombies atacan animales también
Un zombie en el gallinero mata todo en minutos
Los perros detectan zombies antes que el jugador
Los animales domésticos son recurso valioso y vulnerable
```

---

## SISTEMA DE TEMPERATURA

**Variables externas:**
```
→ Temperatura del bioma
→ Estación del año
→ Hora del día — noches más frías
→ Clima — lluvia baja temperatura, sol la sube
→ Viento — aumenta sensación térmica de frío
→ Altitud — más alto más frío
→ Estar mojado — acelera pérdida de calor drásticamente
```

**Variables internas:**
```
→ Ropa equipada — materiales y capas
→ Actividad física — correr genera calor
→ Alimentación — grasas dan resistencia al frío
→ Estado de salud — enfermo pierde regulación térmica
→ Estar cerca de fuego o fuente de calor
→ Estar en interior vs exterior
```

**Zonas de temperatura:**
```
MUY FRÍO   → montañas altas, bioma ártico — hipotermia en minutos
FRÍO       → invierno, noches — ropa de abrigo necesaria
TEMPLADO   → zona de confort — sin penalidades ni bonus
CALOR      → biomas desérticos, verano — hidratación crítica
MUY CALIENTE → zonas volcánicas, incendios — daño directo
```

**Sistema de ropa:**
```
Cada prenda tiene stats:
→ Abrigo         → resistencia al frío
→ Impermeabilidad → resistencia a mojarse
→ Ventilación    → resistencia al calor
→ Protección     → resistencia a daño físico
→ Sigilo         → ruido al moverse
→ Peso           → afecta stamina

CAPAS:
Interior  → ropa interior térmica o ligera
Media     → camisa, pantalón
Exterior  → campera, impermeable, armadura

Armadura completa en verano → golpe de calor garantizado
Ropa liviana en invierno    → hipotermia garantizada
```

**Consecuencias:**
```
HIPOTERMIA:
Etapa 1 → temblores, stamina reducida
Etapa 2 → movimiento lento, visión afectada
Etapa 3 → confusión, no podés correr
Etapa 4 → pérdida de consciencia → muerte

GOLPE DE CALOR:
Etapa 1 → sed extrema, stamina reducida
Etapa 2 → mareos, visión borrosa
Etapa 3 → vómitos, no podés correr
Etapa 4 → colapso → muerte

MOJADO + FRÍO:
→ La combinación más peligrosa
→ Hipotermia avanza 3 veces más rápido
→ Secarse es prioridad absoluta
```

**Fuentes de calor:**
```
Fuego de campamento   → portátil, hace humo y luz
Estufa a leña         → eficiente para interior
Estufa a carbón       → más eficiente, más difícil de conseguir
Calefacción eléctrica → más cómoda, necesita electricidad
Ropa térmica          → calor portátil sin fuente externa
Actividad física      → temporal, cansa
Otro cuerpo           → NPCs aliados dan calor en refugio pequeño
```


---

## INTERFAZ DE USUARIO (UI)

**Filosofía:** Lo mejor de Minecraft y PZ — mínimo siempre visible, todo disponible cuando se necesita.

**SIEMPRE VISIBLE:**
```
HOTBAR DINÁMICA (inferior centro):
→ 1 slot de mano (arma o herramienta)
→ Bolsillos del pantalón (+2 o +4 según pantalón)
→ Argollas de mochila (+2 o +4 según mochila)
→ Barra de peso de mochila al lado del nombre
→ Icono de estado de carga: 🔵 media 🟡 pesada 🔴 sobrecarga

SALUD (inferior izquierda):
→ Iconos de corazón — cambian color según estado
→ Rojo brillante → sano
→ Rojo oscuro    → herido
→ Negro          → crítico
→ Amarillo       → enfermo
→ Azul           → hipotermia
→ Naranja        → golpe de calor

RELOJ (superior derecha):
→ Hora del día y día de la semana
→ Más oscuro de noche
```

**ICONOS SUPERIORES IZQUIERDA (como PZ):**
```
→ ❤️  Panel de salud        (H)
→ 🎒  Inventario            (Tab)
→ 👥  Facción               (F)
→ 🗺️  Mapa                  (M)
→ 📊  Stats y habilidades   (S)
→ ⚙️  Opciones              (Escape)
Parpadean si hay algo urgente
```

**INDICADORES CONTEXTUALES (aparecen con condición):**
```
→ 🍖 Hambre      → hambre moderada
→ 💧 Sed         → sed moderada
→ 🌡️ Temperatura → zona de riesgo
→ 🤒 Enfermedad  → síntomas Etapa 1+
→ ⚡ Stamina     → al correr o pelear
```

---

## PANEL DE SALUD (H)

```
Híbrido — silueta del cuerpo + lista lateral

SILUETA:
→ Partes del cuerpo con colores
→ 🟢 Verde    → sano
→ 🟡 Amarillo → magullado
→ 🔴 Rojo     → herida activa
→ ⚫ Negro    → crítico
→ Click en parte → detalle en lista lateral

LISTA LATERAL:
→ Estado general (temperatura, hidratación, nutrición, descanso)
→ Heridas activas con tipo, severidad y tratamiento
→ Enfermedades activas
→ Medicamentos activos
```

---

## PANEL DE STATS Y HABILIDADES (S)

```
CONDICIÓN FÍSICA:
→ Masa muscular, resistencia, agilidad, peso corporal
→ Barras visuales con descripción

HABILIDADES DESBLOQUEADAS:
→ Sin números ni barras de progreso
→ Solo ves lo que ya sabés hacer
→ Los ??? crean curiosidad
→ Organizadas por categoría: Combate, Supervivencia, Artesanía, Medicina, etc.
```

---

## INVENTARIO — GRILLA TETRIS + PESO (Tab)

```
Cada item tiene tamaño en slots Y peso
Dos límites simultáneos:
→ Espacio (grilla Tetris)
→ Peso (barra de kg)

No podés guardar si:
→ No hay espacio en la grilla
→ O el peso supera el límite aunque haya espacio

PESO PROGRESIVO:
0-50%   → sin penalidad
50-75%  → stamina se consume un poco más rápido
75-90%  → movimiento más lento
90-100% → muy lento, stamina casi nula
```

---

## PANEL DE CRAFTEO (C)

```
LADO IZQUIERDO:
→ Buscador de texto
→ Lista de recetas conocidas
→ Ingredientes en verde (tenés) o rojo (falta)
→ Indicador de ubicación necesaria

LADO DERECHO:
→ Grilla libre para combinación manual
→ Experimenter arrastrando items
→ Si combinación existe → aparece resultado

AL CRAFTEAR:
→ Barra de progreso
→ Tiempo restante
→ Botón cancelar

DÓNDE SE PUEDE CRAFTEAR:
→ Manos        → items básicos de supervivencia
→ Superficie   → armas y herramientas simples
→ Taller básico → armas y herramientas de calidad
→ Taller completo → metal, armaduras, armas de fuego
→ Cocina       → comida, medicamentos, venenos
→ Laboratorio  → químicos, explosivos, medicamentos avanzados

TIEMPO DE CRAFTEO:
→ Instantáneo → items de 1-2 pasos simples
→ Corto       → flechas, trampas, comida simple
→ Medio       → armas de madera, ropa básica
→ Largo       → armas de metal, armaduras
→ Muy largo   → items de altísima calidad, vehículos modificados

DESCUBRIMIENTO DE RECETAS:
→ Buscador de recetas conocidas
→ Combinación manual — experimentando
→ Libros y manuales — acortan tiempo entre boosts
→ Maestros NPC — más rápido, más difícil de conseguir
→ Inspeccionando items ya fabricados

FALLO AL CRAFTEAR:
→ Éxito perfecto   → calidad máxima
→ Éxito normal     → item estándar
→ Éxito parcial    → menor calidad o durabilidad
→ Fallo parcial    → perdés algunos ingredientes
→ Fallo total      → perdés todo
→ Fallo peligroso  → explosivos/químicos pueden lastimarte
```

---

## MUERTE Y PERMADEATH

```
El jugador elige al crear partida:

PERMADEATH:
→ Al morir se borra la partida
→ El mundo sigue — otro personaje puede encontrar tus restos y base

ESTÁNDAR:
→ Respawn en última cama/punto de descanso
→ Perdés todo lo que llevabas encima
→ El mundo avanzó mientras estabas inconsciente
→ Tu base puede haber sido saqueada

SUPERVIVIENTE (intermedio):
→ Quedás inconsciente en el lugar
→ Un aliado NPC o jugador puede salvarte
→ Si nadie llega en X tiempo → muerte permanente
```

---

## MISIONES PROCEDURALES

```
No hay historia principal — sandbox puro
Las misiones son opcionales y generadas por el mundo

ORIGEN:
→ Facciones — necesidades del grupo
→ NPCs individuales — problemas personales
→ Radio — señales de socorro
→ Notas encontradas en el mundo
→ El mundo mismo — hordas, ataques, desastres

TIPOS:
→ Rescate, búsqueda, escolta, defensa
→ Exploración, eliminación, comercio

CONSECUENCIAS REALES:
→ Si fallás o ignorás → el mundo cambia
→ El NPC puede morir, el asentamiento puede caer
→ Las facciones recuerdan
→ Las misiones expiran — el mundo no te espera
```

---

## ZOMBIES ESPECIALES Y JEFES

**Zombies especiales raros — en cualquier zona:**
```
PACIENTE CERO   → primer infectado de la zona, más fuerte, lore
MUTADO          → expuesto a químicos, muy resistente, loot único
BLINDADO        → equipo fusionado al cuerpo, vulnerable por detrás
SILENCIOSO      → sin ruido, solo detectable por olfato o visión
GRITADOR        → llama horda al detectarte, prioridad eliminar
```

**Jefes en zonas especiales:**
```
DIRECTOR DEL LABORATORIO → científico mutado, loot: fórmulas y datos del virus
COMANDANTE MILITAR       → soldado élite, loot: armas militares
CARNICERO                → enorme, herramientas industriales, loot: materiales raros
EL MÉDICO                → equipo médico integrado, loot: medicamentos raros
```

---

## ZONAS ESPECIALES

**Fijas — siempre en el mismo lugar:**
```
→ Laboratorio zona cero (origen del virus)
→ Base militar principal
→ Hospital central
→ Bunker gubernamental
```

**Procedurales — generadas en cada partida:**
```
→ Bunkers improvisados de supervivientes
→ Laboratorios secundarios
→ Bases militares menores
→ Refugios subterráneos
→ Cuevas con historia
```

**Interior:**
```
→ Múltiples niveles subterráneos
→ Puertas con acceso especial
→ Trampas originales del lugar
→ Lore ambiental — notas, grabaciones, experimentos
→ Loot único
→ Jefe al final o en zona de máxima seguridad
```

---

## SISTEMA OFFSCREEN — MUNDO INDEPENDIENTE

```
EL MAPA COMPLETO EXISTE DESDE EL INICIO
→ Las semillas de ruido definen todo el mundo
→ Las coordenadas son reales aunque no estén renderizadas
→ El sistema offscreen trabaja sobre coordenadas, no chunks cargados

COORDENADAS NO VISITADAS:
→ El sistema asigna eventos a coordenadas del mapa completo
→ No necesita renderizar nada — solo datos numéricos
→ Todo guardado en .hesave como datos puros

CUANDO EL JUGADOR LLEGA:
→ El generador lee todos los eventos ocurridos en esa zona
→ Construye el terreno Y el estado actual basado en esa historia
→ El mundo no gira alrededor del jugador
→ El apocalipsis pasó y sigue pasando, con o sin vos
```

---

## SISTEMA DE RUIDO Y SIGILO

```
RUIDO QUE GENERÁS:
→ Caminar normal   → bajo
→ Correr           → alto
→ Agacharse        → mínimo
→ Disparar         → extremo, atrae horda
→ Construcción     → alto
→ Vehículo         → muy alto
→ Ropa metálica    → más ruido al moverse

SIGILO:
→ Agacharse reduce visibilidad y ruido
→ Moverse despacio anula casi todo el ruido
→ Viento afecta si te huelen
→ Cubrirse de barro reduce olor
→ Ropa silenciosa reduce ruido

DETECCIÓN DE ZOMBIES:
→ Vista — línea de visión
→ Oído  — radio según ruido generado
→ Olfato — viento, sangre, suciedad, higiene
```

---

## SISTEMA DE HIGIENE

```
SUCIEDAD:
→ Se acumula con tiempo y combate
→ Sangre de zombies aumenta olor
→ Sin lavar → riesgo de infección en heridas
→ Sin lavar → NPCs reaccionan negativamente
→ Sin lavar → zombies te detectan más fácil

LAVARSE:
→ Agua limpia      → higiene básica
→ Jabón + agua     → higiene completa
→ Río o lago       → funciona, agua puede contaminar
→ Lluvia           → limpieza parcial gratuita
```

---

## SISTEMA DE FUEGO

```
PROPAGACIÓN:
→ Se propaga a materiales inflamables cercanos
→ Madera > tela > pasto > metal (no arde)
→ Viento acelera, lluvia apaga
→ Sin control puede destruir una zona entera

USOS:
→ Cocinar y hervir agua
→ Calefacción y luz
→ Señal — humo atrae zombies Y supervivientes
→ Defensa — barrera de fuego
→ Ataque — flechas incendiarias, molotov
→ Limpieza — quemar cadáveres evita enfermedades

PELIGROS:
→ Tu base puede incendiarse
→ Zombies en llamas propagan el fuego
→ Incendios forestales en biomas secos
```

---

## SISTEMA DE REPUTACIÓN

```
ESCALA -100 a +100 con cada facción:
-100 a -50  → Enemigo — atacan a vista
-50 a -10   → Hostil — desconfianza, precios abusivos
-10 a +10   → Neutral — comercio básico
+10 a +50   → Aliado — mejores precios, información
+50 a +100  → Hermano — comparten recursos, defienden base

CAMBIA POR:
→ Misiones completadas o falladas
→ Comercio honesto o estafas
→ Atacar o ayudar a sus miembros
→ Rumores y decisiones en conflictos

NPCs INDIVIDUALES:
→ Memoria propia independiente de la facción
→ Un NPC puede ser amigo aunque su facción te odie
```

---

## RADIO Y COMUNICACIÓN

```
TIPOS:
→ Radio portátil  → recibís señales, corto alcance
→ Radio de base   → recibís y enviás, largo alcance
→ Radio militar   → frecuencias cifradas, equipos raros

USOS:
→ Señales de socorro — misiones procedurales
→ Comunicación con facciones aliadas
→ Transmisiones del mundo — lore
→ Señales misteriosas de zonas subterráneas
→ Propaganda de facciones

EL MUNDO A TRAVÉS DE LA RADIO:
→ Transmisiones grabadas del gobierno antes del colapso
→ Señales de socorro sin respuesta
→ Señales inexplicables que vienen de muy abajo...
```

---

## CLIMA EXTREMO EN TIEMPO REAL

```
TORNADO:
→ Aviso previo — cielo cambia, animales huyen
→ Sin refugio → daño masivo, te lanza
→ Destruye construcciones débiles
→ Zona devastada con loot disperso después

HURACÁN/TORMENTA SEVERA:
→ Dura horas de juego
→ Viento extremo, lluvia torrencial
→ Inundaciones en zonas bajas
→ Los zombies también se ven afectados

NEVADA EXTREMA:
→ Visibilidad casi nula
→ Temperatura cae drásticamente
→ Vehículos pueden quedar atascados
→ Zombies más lentos — ventaja táctica

TERREMOTO:
→ Sin aviso previo
→ Construcciones pueden colapsar
→ Terreno se modifica — grietas, derrumbes
→ Puede abrir cuevas antes cerradas

ERUPCIÓN VOLCÁNICA:
→ Aviso largo — días con señales previas
→ Lluvia de ceniza — problemas respiratorios
→ Lava — zonas inaccesibles permanentemente
→ Crea nuevos recursos — obsidiana, minerales
```


---

## SIMULACIÓN ACÚSTICA — RAY-TRACING DE SONIDO

**Idea original aportada por Gemini AI**

```
El sonido no es un porcentaje de alerta — es física real

COEFICIENTES DE ABSORCIÓN POR MATERIAL:
→ Puerta de madera        → atenúa 60% del sonido
→ Puerta reforzada        → atenúa 90%
→ Pared de ladrillo       → atenúa 85%
→ Pared de metal          → atenúa 95%
→ Ventana rota            → atenúa 10%
→ Ventana cerrada         → atenúa 70%
→ Pared de madera         → atenúa 50%
→ Espacio abierto         → sin atenuación, se propaga en radio

COMPORTAMIENTO:
→ Un disparo en habitación cerrada no alerta al zombie del pasillo
   si la puerta está reforzada
→ Golpear una pared se escucha al otro lado
→ El sonido rebota en superficies duras
→ Los materiales blandos (tela, tierra) absorben más
→ El viento transporta el sonido en su dirección

CONSECUENCIA DE DISEÑO:
→ El sigilo es arquitectura y física, no azar
→ Construís tu base pensando en la acústica
→ Un taller ruidoso necesita paredes gruesas
→ Una sala de radio necesita aislamiento
→ Las puertas reforzadas valen su peso en oro acústico
```

---

## ECOSISTEMAS DEVORADORES

**Idea original aportada por Gemini AI**

```
El mundo no solo se deteriora — también vive y crece

VEGETACIÓN ACTIVA:
→ Las raíces crecen con el tiempo
→ Construcciones abandonadas → raíces rompen cimientos
→ Sin mantenimiento regular → la base se deteriora desde abajo
→ Cada material tiene resistencia diferente a las raíces
   Madera → muy vulnerable
   Ladrillo → resistente pero no inmune
   Metal → casi inmune pero se oxida igual

NATURALEZA RECLAMANDO ZONAS:
→ Si una zona se queda sin zombies y sin humanos
→ La vegetación la recupera gradualmente
→ Semanas → arbustos crecen, dificultan visión
→ Meses → árboles jóvenes, calles cubiertas
→ Años → la ciudad original casi irreconocible
→ Crea cobertura natural para el sigilo
→ Pero también esconde peligros

FAUNA COMPETITIVA:
→ Los animales forman manadas propias
→ Compiten con los zombies por recursos y territorio
→ Una manada de perros puede limpiar zombies de una zona
→ O puede ser tan peligrosa como ellos
→ El ecosistema se equilibra solo con o sin el jugador

PROCESAMIENTO:
→ Todo offscreen durante el sueño del jugador
→ Coordenadas sin actividad humana → naturaleza avanza
→ Coordenadas con base activa → naturaleza contenida
   si el jugador la mantiene
```

---

## IA DE PERSONALIDAD TRAUMÁTICA — MEMORIA EPISÓDICA

**Idea original aportada por Gemini AI**

```
Los NPCs no solo recuerdan hechos — recuerdan experiencias

ARCHIVO DE MEMORIA EPISÓDICA (Python):
→ Cada NPC tiene un registro de eventos que vivió
→ No es solo un número de reputación
→ Es una lista de momentos específicos con contexto

EJEMPLO DE MEMORIA:
{
  "evento": "vio al jugador matar civil por suministros",
  "fecha_juego": "día 45",
  "impacto_confianza": -40,
  "accion_futura": "deserción posible si confianza < 20",
  "tiempo_procesamiento": "entre día 60 y día 90"
}

CONSECUENCIAS A LARGO PLAZO:
→ Un NPC rescatado te es leal — recuerda que le salvaste la vida
→ Un NPC que vio algo perturbador puede desertar meses después
→ Al desertar puede llevar tus coordenadas a enemigos
→ Puede revelar tus rutas, tus recursos, tus aliados
→ El jugador nunca sabe exactamente cuándo ni si pasará

TIPOS DE MEMORIAS QUE AFECTAN AL NPC:
→ Rescates y ayuda          → confianza +
→ Abandono en peligro       → confianza --
→ Ver violencia injustificada → confianza --
→ Compartir recursos escasos → lealtad +
→ Mentiras descubiertas      → confianza -- permanente
→ Pérdida de seres queridos  → trauma, comportamiento errático
→ Victoria compartida        → vínculo fuerte

PROCESAMIENTO:
→ Las memorias se acumulan durante el juego
→ Python las procesa durante el sueño del jugador
→ Calcula si el NPC tomará alguna acción
→ Las acciones pueden ser inmediatas o diferidas meses
→ Crea narrativa emergente sin scripting manual
```
