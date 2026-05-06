#include "inventario.h"
#include <algorithm>

Inventario::Inventario() {
    pesoActual       = 0.0f;
    abierto          = false;
    slotSeleccionado = -1;
}

int Inventario::buscarSlotApilable(TipoItem tipo) const {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (slots[i].tipo == tipo && slots[i].cantidad < slots[i].stackMax)
            return i;
    }
    return -1;
}

int Inventario::buscarSlotVacio() const {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (!slots[i].esValido()) return i;
    }
    return -1;
}

bool Inventario::agregarItem(Item item) {
    // Intentar apilar primero
    int slotApilable = buscarSlotApilable(item.tipo);
    if (slotApilable >= 0) {
        int espacio = slots[slotApilable].stackMax - slots[slotApilable].cantidad;
        int agregar = std::min(item.cantidad, espacio);
        slots[slotApilable].cantidad += agregar;
        pesoActual += item.peso * agregar;
        return true;
    }

    // Si no se puede apilar, buscar slot vacío
    int slotVacio = buscarSlotVacio();
    if (slotVacio < 0) return false;  // inventario lleno

    slots[slotVacio] = item;
    pesoActual += item.peso * item.cantidad;
    return true;
}

Item Inventario::usarItem(int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return Item{};
    if (!slots[slot].esValido()) return Item{};

    Item usado = slots[slot];
    usado.cantidad = 1;

    slots[slot].cantidad--;
    pesoActual -= slots[slot].peso;

    if (slots[slot].cantidad <= 0) {
        slots[slot] = Item{};  // limpiar slot
    }

    return usado;
}

void Inventario::removerItem(int slot) {
    if (slot < 0 || slot >= MAX_SLOTS) return;
    if (!slots[slot].esValido()) return;
    pesoActual -= slots[slot].peso * slots[slot].cantidad;
    slots[slot] = Item{};
}