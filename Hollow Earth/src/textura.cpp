#include "textura.h"
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

Textura::Textura() : id(0) {}

Textura::Textura(const std::string& rutaArchivo, const std::string& tipoTex) 
    : tipo(tipoTex), ruta(rutaArchivo) {
    cargar(rutaArchivo);
}

Textura::~Textura() {
    if (id != 0) {
        glDeleteTextures(1, &id);
    }
}

void Textura::cargar(const std::string& rutaArchivo) {
    std::cout << "Cargando textura: " << rutaArchivo << std::endl;
    
    // Cargar imagen con SDL_image
    SDL_Surface* superficie = IMG_Load(rutaArchivo.c_str());
    if (!superficie) {
        std::cout << "Error cargando imagen: " << SDL_GetError() << std::endl;
        std::cout << "Creando textura de respaldo..." << std::endl;
        crearTexturaRespaldo();
        return;
    }
    
    // Determinar formato según canales
    GLenum formato;
    if (SDL_BYTESPERPIXEL(superficie->format) == 4) {
        formato = GL_RGBA;
    } else {
        formato = GL_RGB;
    }
    
    // Generar textura OpenGL
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, formato, 
                 superficie->w, superficie->h, 0, 
                 formato, GL_UNSIGNED_BYTE, superficie->pixels);
    
    // Configurar parámetros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Generar mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    std::cout << "Textura cargada: " << superficie->w << "x" << superficie->h 
              << " canales: " << (int)SDL_BYTESPERPIXEL(superficie->format) << std::endl;
    
    SDL_DestroySurface(superficie);
}

void Textura::crearTexturaRespaldo() {
    // Crear textura de ajedrez como respaldo
    int ancho = 64, alto = 64;
    std::vector<unsigned char> datos(ancho * alto * 3);
    
    for (int y = 0; y < alto; y++) {
        for (int x = 0; x < ancho; x++) {
            int i = (y * ancho + x) * 3;
            bool esBlanco = ((x / 8) + (y / 8)) % 2 == 0;
            
            if (esBlanco) {
                datos[i] = 255;
                datos[i+1] = 255;
                datos[i+2] = 255;
            } else {
                datos[i] = 100;
                datos[i+1] = 150;
                datos[i+2] = 100;
            }
        }
    }
    
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ancho, alto, 0, GL_RGB, GL_UNSIGNED_BYTE, datos.data());
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    std::cout << "Textura de respaldo creada" << std::endl;
}

void Textura::usar(unsigned int unidad) const {
    if (id != 0) {
        glActiveTexture(GL_TEXTURE0 + unidad);
        glBindTexture(GL_TEXTURE_2D, id);
    }
}