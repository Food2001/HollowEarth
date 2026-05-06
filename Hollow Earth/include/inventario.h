#pragma once
#include <string>
#include <vector>

// ================================================
// Tipos de item
// ================================================
enum class TipoItem {
    NINGUNO,
    VENDA,       // cura 25 de salud
    KIT_MEDICO,  // cura 60 de salud
};

struct Item {
    TipoItem    tipo   = TipoItem::NINGUNO;
    std::string nombre = "";
    float       peso   = 0.0f;
    int         cantidad = 0;
    int         stackMax = 1;  // máximo apilable

    bool esValido() const { return tipo != TipoItem::NINGUNO && cantidad > 0; }
};

// ================================================
// Inventario — grilla 6 columnas x 5 filas = 30 slots
// ================================================
class Inventario {
public:
    static constexpr int COLUMNAS   = 6;
    static constexpr int FILAS      = 5;
    static constexpr int MAX_SLOTS  = COLUMNAS * FILAS;  // 30
    static constexpr float PESO_MAX = 30.0f;  // kg

    Inventario();

    // Agregar item — devuelve true si hubo espacio
    bool agregarItem(Item item);

    // Usar item en slot dado — devuelve el item para aplicar efecto
    Item usarItem(int slot);

    // Remover un item del slot
    void removerItem(int slot);

    float getPesoActual() const { return pesoActual; }
    float getPesoPorcentaje() const { return pesoActual / PESO_MAX; }

    const Item& getSlot(int slot) const { return slots[slot]; }
    bool estaAbierto() const { return abierto; }
    void toggleAbierto() { abierto = !abierto; }

    int getSlotSeleccionado() const { return slotSeleccionado; }
    void setSlotSeleccionado(int s) { slotSeleccionado = s; }

private:
    Item  slots[MAX_SLOTS];
    float pesoActual;
    bool  abierto;
    int   slotSeleccionado;

    // Buscar slot con mismo tipo para apilar
    int buscarSlotApilable(TipoItem tipo) const;
    // Buscar primer slot vacío
    int buscarSlotVacio() const;
};

// ================================================
// Tabla de datos de items
// ================================================
namespace DatosItem {
    inline Item crearVenda() {
        Item i;
        i.tipo     = TipoItem::VENDA;
        i.nombre   = "Venda";
        i.peso     = 0.2f;
        i.cantidad = 1;
        i.stackMax = 5;
        return i;
    }

    inline Item crearKitMedico() {
        Item i;
        i.tipo     = TipoItem::KIT_MEDICO;
        i.nombre   = "Kit Medico";
        i.peso     = 0.8f;
        i.cantidad = 1;
        i.stackMax = 3;
        return i;
    }

    inline float getCuracion(TipoItem tipo) {
        switch (tipo) {
            case TipoItem::VENDA:      return 0.25f;  // 25% de salud
            case TipoItem::KIT_MEDICO: return 0.60f;  // 60% de salud
            default: return 0.0f;
        }
    }

    // Color de fondo del slot según tipo
    inline void getColor(TipoItem tipo, float& r, float& g, float& b) {
        switch (tipo) {
            case TipoItem::VENDA:
                r=0.8f; g=0.2f; b=0.2f; break;  // rojo (médico)
            case TipoItem::KIT_MEDICO:
                r=0.9f; g=0.1f; b=0.1f; break;  // rojo oscuro
            default:
                r=0.3f; g=0.3f; b=0.3f; break;  // gris vacío
        }
    }
}