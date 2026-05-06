#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

class Textura {
public:
    unsigned int id;
    std::string tipo;
    std::string ruta;

    Textura();
    Textura(const std::string& rutaArchivo, const std::string& tipoTex = "diffuse");
    ~Textura();

    void cargar(const std::string& rutaArchivo);
    void usar(unsigned int unidad = 0) const;
    
private:
    void crearTexturaRespaldo();
};