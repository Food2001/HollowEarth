#version 330 core
in vec3  Normal;
in vec3  PosicionMundo;
in float Visibilidad;
in vec2  UV;
in float Altura;

out vec4 colorFinal;

uniform vec3  luzPosicion;
uniform vec3  luzColor;
uniform float luzIntensidad;
uniform vec3  ambientColor;
uniform float ambientIntensidad;

uniform vec3  nieblaColor;
uniform bool  nieblaHabilitada;

uniform sampler2D texturaDifusa;
uniform bool      usarTextura;
uniform vec3      colorSolido;
uniform float     alpha;

uniform bool      usarMultiTextura;
uniform sampler2D texturaArena;
uniform sampler2D texturaPasto;
uniform sampler2D texturaRoca;
uniform sampler2D texturaNieve;

float mezclarAltura(float h, float min1, float max1) {
    return clamp((h - min1) / (max1 - min1), 0.0, 1.0);
}

void main() {
    vec3 norm = normalize(Normal);

    vec3 ambient = ambientIntensidad * ambientColor;

    vec3  luzDir  = normalize(luzPosicion - PosicionMundo);
    float diff    = max(dot(norm, luzDir), 0.0);
    float sombra  = mix(0.0, 1.0, smoothstep(0.0, 0.3, diff));
    vec3  diffuse = sombra * luzColor * luzIntensidad;

    vec3 colorBase;

    if (usarMultiTextura) {
        vec2 uvTerreno = UV * 6.0;

        vec3 cArena = texture(texturaArena, uvTerreno).rgb;
        vec3 cPasto = texture(texturaPasto, uvTerreno).rgb;
        vec3 cRoca  = texture(texturaRoca,  uvTerreno).rgb;
        vec3 cNieve = texture(texturaNieve, uvTerreno).rgb;

        float pendiente           = 1.0 - dot(norm, vec3(0.0, 1.0, 0.0));
        float factorRocaPendiente = smoothstep(0.4, 0.7, pendiente);

        float tArena = 1.0 - mezclarAltura(Altura,  0.0,  3.0);
        float tPasto =       mezclarAltura(Altura,  0.0,  3.0)
                     * (1.0 - mezclarAltura(Altura, 5.0,  8.0));
        float tRoca  =       mezclarAltura(Altura,  5.0,  8.0)
                     * (1.0 - mezclarAltura(Altura,10.0, 14.0));
        float tNieve =       mezclarAltura(Altura, 10.0, 14.0);

        colorBase = cArena * tArena
                  + cPasto * tPasto
                  + cRoca  * tRoca
                  + cNieve * tNieve;

        colorBase = mix(colorBase, cRoca, factorRocaPendiente);

    } else if (usarTextura) {
        colorBase = texture(texturaDifusa, UV).rgb;
    } else {
        colorBase = colorSolido;
    }

    vec3 resultado = (ambient + diffuse) * colorBase;

    if (nieblaHabilitada) {
        resultado = mix(nieblaColor, resultado, Visibilidad);
    }

    colorFinal = vec4(resultado, alpha);
}