#ifndef TABLAHASH_H
#define TABLAHASH_H

#include "Estudiante.h"
#include <vector>

struct NodoHash {
    Estudiante* estudiante;
    NodoHash* next;

    NodoHash(Estudiante* est) : estudiante(est), next(nullptr) {}
};

class TablaHash {
private:
    NodoHash** tabla;
    int capacidad;
    int total_elementos;
    int total_colisiones;

    // Función hash simple por módulo (los IDs son enteros)
    int funcionHash(int id) const {
        return id % capacidad;
    }

public:
    TablaHash(int cap = 1009) : capacidad(cap), total_elementos(0), total_colisiones(0) {
        tabla = new NodoHash*[capacidad]();
    }

    ~TablaHash() {
        for (int i = 0; i < capacidad; ++i) {
            NodoHash* actual = tabla[i];
            while (actual != nullptr) {
                NodoHash* temp = actual;
                actual = actual->next;
                delete temp;
               }
        }
        delete[] tabla;
    }

    bool insertar(Estudiante* est) {
        if (!est) return false;

        int indice = funcionHash(est->student_id);
        NodoHash* actual = tabla[indice];

        // Validación obligatoria: Evitar ID duplicado
        while (actual != nullptr) {
            if (actual->estudiante->student_id == est->student_id) {
                return false;
            }
            actual = actual->next;
        }

        // Si ya hay elementos en este bucket, es una colisión
        if (tabla[indice] != nullptr) {
            total_colisiones++;
        }

        // Insertar al inicio de la lista (O(1))
        NodoHash* nuevoNodo = new NodoHash(est);
        nuevoNodo->next = tabla[indice];
        tabla[indice] = nuevoNodo;
        total_elementos++;
        return true;
    }

    Estudiante* buscar(int id) const {
        int indice = funcionHash(id);
        NodoHash* actual = tabla[indice];

        while (actual != nullptr) {
            if (actual->estudiante->student_id == id) {
                return actual->estudiante;
            }
            actual = actual->next;
        }
        return nullptr;
    }

    bool eliminar(int id) {
        int indice = funcionHash(id);
        NodoHash* actual = tabla[indice];
        NodoHash* anterior = nullptr;

        while (actual != nullptr) {
            if (actual->estudiante->student_id == id) {
                if (anterior == nullptr) {
                    // El elemento a eliminar es el primero de la lista
                    tabla[indice] = actual->next;
                } else {
                    anterior->next = actual->next;
                }

                delete actual;
                total_elementos--;
                return true;
            }
            anterior = actual;
            actual = actual->next;
        }
        return false;
    }

    // Métodos estadísticos obligatorios
    float getFactorCarga() const {
        return static_cast<float>(total_elementos) / capacidad;
    }

    int getColisiones() const {
        return total_colisiones;
    }

    int getTotalElementos() const {
        return total_elementos;
    }
};

#endif