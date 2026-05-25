#ifndef ARBOLAVL_H
#define ARBOLAVL_H

#include "Estudiante.h"
#include <algorithm>
#include <iostream>

struct NodoAVL {
    Estudiante* estudiante;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    NodoAVL(Estudiante* est) : estudiante(est), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

class ArbolAVL {
private:
    NodoAVL* raiz;

    int getAltura(NodoAVL* nodo) const { return nodo == nullptr ? 0 : nodo->altura; }
    int getFactorEquilibrio(NodoAVL* nodo) const { return nodo == nullptr ? 0 : getAltura(nodo->izquierdo) - getAltura(nodo->derecho); }

    NodoAVL* rotacionDerecha(NodoAVL* y) {
        NodoAVL* x = y->izquierdo;
        NodoAVL* T2 = x->derecho;
        x->derecho = y;
        y->izquierdo = T2;
        y->altura = std::max(getAltura(y->izquierdo), getAltura(y->derecho)) + 1;
        x->altura = std::max(getAltura(x->izquierdo), getAltura(x->derecho)) + 1;
        return x;
    }

    NodoAVL* rotacionIzquierda(NodoAVL* x) {
        NodoAVL* y = x->derecho;
        NodoAVL* T2 = y->izquierdo;
        y->izquierdo = x;
        x->derecho = T2;
        x->altura = std::max(getAltura(x->izquierdo), getAltura(x->derecho)) + 1;
        y->altura = std::max(getAltura(y->izquierdo), getAltura(y->derecho)) + 1;
        return y;
    }

    NodoAVL* insertarRecursivo(NodoAVL* nodo, Estudiante* est) {
        if (nodo == nullptr) return new NodoAVL(est);
        if (est->skill_score < nodo->estudiante->skill_score) nodo->izquierdo = insertarRecursivo(nodo->izquierdo, est);
        else nodo->derecho = insertarRecursivo(nodo->derecho, est);

        nodo->altura = 1 + std::max(getAltura(nodo->izquierdo), getAltura(nodo->derecho));
        int balance = getFactorEquilibrio(nodo);

        if (balance > 1 && est->skill_score < nodo->izquierdo->estudiante->skill_score) return rotacionDerecha(nodo);
        if (balance < -1 && est->skill_score >= nodo->derecho->estudiante->skill_score) return rotacionIzquierda(nodo);
        if (balance > 1 && est->skill_score >= nodo->izquierdo->estudiante->skill_score) {
            nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
            return rotacionDerecha(nodo);
        }
        if (balance < -1 && est->skill_score < nodo->derecho->estudiante->skill_score) {
            nodo->derecho = rotacionDerecha(nodo->derecho);
            return rotacionIzquierda(nodo);
        }
        return nodo;
    }

    NodoAVL* getNodoMinimo(NodoAVL* nodo) {
        NodoAVL* actual = nodo;
        while (actual->izquierdo != nullptr) actual = actual->izquierdo;
        return actual;
    }

    NodoAVL* eliminarRecursivo(NodoAVL* raiz, int score, int id) {
        if (raiz == nullptr) return raiz;

        if (score < raiz->estudiante->skill_score) raiz->izquierdo = eliminarRecursivo(raiz->izquierdo, score, id);
        else if (score > raiz->estudiante->skill_score) raiz->derecho = eliminarRecursivo(raiz->derecho, score, id);
        else {
            if (raiz->estudiante->student_id != id) raiz->derecho = eliminarRecursivo(raiz->derecho, score, id);
            else {
                if ((raiz->izquierdo == nullptr) || (raiz->derecho == nullptr)) {
                    NodoAVL* temp = raiz->izquierdo ? raiz->izquierdo : raiz->derecho;
                    if (temp == nullptr) { temp = raiz; raiz = nullptr; }
                    else { *raiz = *temp; }
                    delete temp;
                } else {
                    NodoAVL* temp = getNodoMinimo(raiz->derecho);
                    raiz->estudiante = temp->estudiante;
                    raiz->derecho = eliminarRecursivo(raiz->derecho, temp->estudiante->skill_score, temp->estudiante->student_id);
                }
            }
        }

        if (raiz == nullptr) return raiz;

        raiz->altura = 1 + std::max(getAltura(raiz->izquierdo), getAltura(raiz->derecho));
        int balance = getFactorEquilibrio(raiz);

        if (balance > 1 && getFactorEquilibrio(raiz->izquierdo) >= 0) return rotacionDerecha(raiz);
        if (balance > 1 && getFactorEquilibrio(raiz->izquierdo) < 0) {
            raiz->izquierdo = rotacionIzquierda(raiz->izquierdo); return rotacionDerecha(raiz);
        }
        if (balance < -1 && getFactorEquilibrio(raiz->derecho) <= 0) return rotacionIzquierda(raiz);
        if (balance < -1 && getFactorEquilibrio(raiz->derecho) > 0) {
            raiz->derecho = rotacionDerecha(raiz->derecho); return rotacionIzquierda(raiz);
        }
        return raiz;
    }

    void inOrdenRecursivo(NodoAVL* nodo, bool silencioso = false) const {
        if (nodo != nullptr) {
            inOrdenRecursivo(nodo->derecho, silencioso);
            if (!silencioso) {
                std::cout << "Score: " << nodo->estudiante->skill_score
                          << " | ID: " << nodo->estudiante->student_id
                          << " | Nombre: " << nodo->estudiante->full_name << std::endl;
            }
            // Variable temporal estática para forzar acceso a memoria en el benchmark sin ser optimizada por el compilador
            volatile int dummy = nodo->estudiante->skill_score;
            inOrdenRecursivo(nodo->izquierdo, silencioso);
        }
    }

    void liberarMemoria(NodoAVL* nodo) {
        if (nodo != nullptr) {
            liberarMemoria(nodo->izquierdo);
            liberarMemoria(nodo->derecho);
            delete nodo;
        }
    }

public:
    ArbolAVL() : raiz(nullptr) {}
    ~ArbolAVL() { liberarMemoria(raiz); }

    void insertar(Estudiante* est) { if (est != nullptr) raiz = insertarRecursivo(raiz, est); }
    void eliminar(int score, int id) { raiz = eliminarRecursivo(raiz, score, id); }

    void mostrarRanking() const {
        std::cout << "\n--- RANKING ACADEMICO ---" << std::endl;
        inOrdenRecursivo(raiz, false);
    }

    // Método expuesto para el benchmark
    void recorridoSilencioso() const { inOrdenRecursivo(raiz, true); }

    int getAlturaTotal() const { return getAltura(raiz); }
};

#endif