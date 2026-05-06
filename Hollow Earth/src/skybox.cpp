#include "skybox.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Shaders propios del skybox — sin model matrix, view sin traslación
static const char* SKY_VERT = R"(
#version 330 core
layout (location = 0) in vec3 posicion;
out vec3 coordTextura;
uniform mat4 view;
uniform mat4 projection;
void main() {
    coordTextura = posicion;
    vec4 pos = projection * view * vec4(posicion, 1.0);
    // Forzar z = w para que el skybox siempre esté en el fondo
    gl_Position = pos.xyww;
}
)";

static const char* SKY_FRAG = R"(
#version 330 core
in vec3 coordTextura;
out vec4 colorFinal;
uniform samplerCube skybox;
void main() {
    colorFinal = texture(skybox, coordTextura);
}
)";

// Vértices del cubo unitario para el skybox
static float SKY_VERTICES[] = {
    -1, -1,  1,   1, -1,  1,   1,  1,  1,   1,  1,  1,  -1,  1,  1,  -1, -1,  1,  // front
    -1, -1, -1,  -1,  1, -1,   1,  1, -1,   1,  1, -1,   1, -1, -1,  -1, -1, -1,  // back
    -1,  1, -1,  -1,  1,  1,   1,  1,  1,   1,  1,  1,   1,  1, -1,  -1,  1, -1,  // top
    -1, -1, -1,   1, -1, -1,   1, -1,  1,   1, -1,  1,  -1, -1,  1,  -1, -1, -1,  // bottom
     1, -1, -1,   1,  1, -1,   1,  1,  1,   1,  1,  1,   1, -1,  1,   1, -1, -1,  // right
    -1, -1, -1,  -1, -1,  1,  -1,  1,  1,  -1,  1,  1,  -1,  1, -1,  -1, -1, -1,  // left
};

// -------------------------------------------------------

unsigned int Skybox::compilarShader(const char* vert, const char* frag) {
    int exito; char log[512];

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vert, nullptr);
    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &exito);
    if (!exito) { glGetShaderInfoLog(v, 512, nullptr, log); std::cout << "Skybox vert: " << log << std::endl; }

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &frag, nullptr);
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &exito);
    if (!exito) { glGetShaderInfoLog(f, 512, nullptr, log); std::cout << "Skybox frag: " << log << std::endl; }

    unsigned int p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

unsigned int Skybox::cargarCubemap(const std::string& carpeta) {
    // Orden que espera OpenGL para cubemaps
    // Ajustado a los nombres de tus archivos
    std::string caras[6] = {
        carpeta + "/clouds1_east.bmp",   // +X
        carpeta + "/clouds1_west.bmp",   // -X
        carpeta + "/clouds1_up.bmp",     // +Y
        carpeta + "/clouds1_down.bmp",   // -Y
        carpeta + "/clouds1_south.bmp",  // +Z
        carpeta + "/clouds1_north.bmp",  // -Z
    };

    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (int i = 0; i < 6; i++) {
        SDL_Surface* img = IMG_Load(caras[i].c_str());
        if (!img) {
            std::cout << "Error skybox cara " << i << ": " << SDL_GetError() << std::endl;
            continue;
        }

        // SDL3_image puede cargar BMP — convertir a formato consistente
        SDL_Surface* convertida = SDL_ConvertSurface(img, SDL_GetPixelFormatForMasks(24, 0xFF0000, 0x00FF00, 0x0000FF, 0));
        SDL_DestroySurface(img);

        if (convertida) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                         convertida->w, convertida->h, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, convertida->pixels);
            std::cout << "Skybox cara " << i << " cargada: " << convertida->w << "x" << convertida->h << std::endl;
            SDL_DestroySurface(convertida);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

Skybox::Skybox(const std::string& carpeta) {
    shaderID  = compilarShader(SKY_VERT, SKY_FRAG);
    cubemapID = cargarCubemap(carpeta);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKY_VERTICES), SKY_VERTICES, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &cubemapID);
    glDeleteProgram(shaderID);
}

void Skybox::dibujar(const glm::mat4& view, const glm::mat4& projection) {
    // Quitar traslación de la view matrix — el skybox no se mueve
    glm::mat4 viewSinTraslacion = glm::mat4(glm::mat3(view));

    // Dibujar detrás de todo — deshabilitar escritura al depth
    glDepthFunc(GL_LEQUAL);

    glUseProgram(shaderID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"),       1, GL_FALSE, glm::value_ptr(viewSinTraslacion));
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(shaderID, "skybox"), 0);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);  // restaurar
}