#version 330 core
layout (location = 0) in vec3 posicion;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

// === INSTANCIADO === Matriz de modelo por instancia (solo árboles, zombies si se usa)
layout (location = 3) in mat4 instanceModel;

out vec3  Normal;
out vec3  PosicionMundo;
out float Visibilidad;
out vec2  UV;
out float Altura;

uniform mat4  model;
uniform mat4  view;
uniform mat4  projection;
uniform float nieblaDensidad;

uniform bool  usarInstancia;   // <-- NUEVO: controla si usar la matriz de instancia

void main() {
    mat4 modelMatrix = model;
    if (usarInstancia) {
        modelMatrix = instanceModel;
    }

    PosicionMundo = vec3(modelMatrix * vec4(posicion, 1.0));
    Normal        = mat3(transpose(inverse(modelMatrix))) * normal;
    UV            = uv;
    Altura        = PosicionMundo.y;

    vec4  posCamara = view * vec4(PosicionMundo, 1.0);
    float dist      = length(posCamara.xyz);
    Visibilidad     = exp(-dist * nieblaDensidad);

    gl_Position = projection * view * vec4(PosicionMundo, 1.0);
}