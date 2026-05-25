#ifndef GRAFO_H
#define GRAFO_H

#include "Estudiante.h"
#include <iostream>

struct Arco {
    Estudiante* destino; Arco* siguiente;
    Arco(Estudiante* dest) : destino(dest), siguiente(nullptr) {}
};

struct Vertice {
    Estudiante* origen; Arco* adyacentes; Vertice* siguiente;
    Vertice(Estudiante* orig) : origen(orig), adyacentes(nullptr), siguiente(nullptr) {}
};

struct NodoRecorrido {
    Estudiante* est; NodoRecorrido* sig;
    NodoRecorrido(Estudiante* e) : est(e), sig(nullptr) {}
};

class ColaBFS {
    NodoRecorrido *frente, *final;
public:
    ColaBFS() : frente(nullptr), final(nullptr) {}
    ~ColaBFS() { while(!estaVacia()) desencolar(); }
    void encolar(Estudiante* e) {
        NodoRecorrido* n = new NodoRecorrido(e);
        if(final) { final->sig = n; final = n; } else { frente = final = n; }
    }
    Estudiante* desencolar() {
        if(!frente) return nullptr;
        NodoRecorrido* temp = frente; Estudiante* e = temp->est;
        frente = frente->sig; if(!frente) final = nullptr;
        delete temp; return e;
    }
    bool estaVacia() const { return frente == nullptr; }
};

class PilaDFS {
    NodoRecorrido* tope;
public:
    PilaDFS() : tope(nullptr) {}
    ~PilaDFS() { while(!estaVacia()) desapilar(); }
    void apilar(Estudiante* e) {
        NodoRecorrido* n = new NodoRecorrido(e); n->sig = tope; tope = n;
    }
    Estudiante* desapilar() {
        if(!tope) return nullptr;
        NodoRecorrido* temp = tope; Estudiante* e = temp->est;
        tope = tope->sig; delete temp; return e;
    }
    bool estaVacia() const { return tope == nullptr; }
};

class Grafo {
private:
    Vertice* primer_vertice;
    int total_conexiones;

public:
    Grafo() : primer_vertice(nullptr), total_conexiones(0) {}

    ~Grafo() {
        Vertice* v = primer_vertice;
        while(v) {
            Arco* a = v->adyacentes;
            while(a) { Arco* tempA = a; a = a->siguiente; delete tempA; }
            Vertice* tempV = v; v = v->siguiente; delete tempV;
        }
    }

    Vertice* buscarVertice(int id) const {
        Vertice* aux = primer_vertice;
        while(aux) { if(aux->origen->student_id == id) return aux; aux = aux->siguiente; }
        return nullptr;
    }

    void insertarVertice(Estudiante* est) {
        if(!est) return;
        Vertice* n = new Vertice(est); n->siguiente = primer_vertice; primer_vertice = n;
    }

    bool conectar(int id1, int id2) {
        if (id1 == id2) return false;
        Vertice* v1 = buscarVertice(id1); Vertice* v2 = buscarVertice(id2);
        if (!v1 || !v2) return false;
        Arco* aux = v1->adyacentes;
        while(aux) { if (aux->destino->student_id == id2) return false; aux = aux->siguiente; }
        Arco* a1 = new Arco(v2->origen); a1->siguiente = v1->adyacentes; v1->adyacentes = a1;
        Arco* a2 = new Arco(v1->origen); a2->siguiente = v2->adyacentes; v2->adyacentes = a2;
        total_conexiones++; return true;
    }

    bool desconectar(int id1, int id2) {
        Vertice* v1 = buscarVertice(id1); Vertice* v2 = buscarVertice(id2);
        if (!v1 || !v2) return false;
        auto removerArco = [](Vertice* v, int id_dest) {
            Arco* actual = v->adyacentes; Arco* anterior = nullptr;
            while (actual) {
                if (actual->destino->student_id == id_dest) {
                    if (anterior) anterior->siguiente = actual->siguiente; else v->adyacentes = actual->siguiente;
                    delete actual; return true;
                }
                anterior = actual; actual = actual->siguiente;
            } return false;
        };
        bool r1 = removerArco(v1, id2); bool r2 = removerArco(v2, id1);
        if (r1 && r2) { total_conexiones--; return true; }
        return false;
    }

    bool estanConectados(int id1, int id2) const {
        Vertice* v1 = buscarVertice(id1); if(!v1) return false;
        Arco* aux = v1->adyacentes;
        while(aux) { if(aux->destino->student_id == id2) return true; aux = aux->siguiente; }
        return false;
    }

    void mostrarConexiones(int id) const {
        Vertice* v = buscarVertice(id);
        if(!v) { std::cout << "Estudiante no encontrado." << std::endl; return; }
        std::cout << "\nConexiones de " << v->origen->full_name << " (" << id << "):" << std::endl;
        Arco* aux = v->adyacentes; int count = 0;
        while(aux) { std::cout << "  -> " << aux->destino->full_name << std::endl; aux = aux->siguiente; count++; }
        if(count == 0) std::cout << "  Sin conexiones." << std::endl;
    }

    void BFS(int start_id, bool silencioso = false) {
        Vertice* inicio = buscarVertice(start_id);
        if(!inicio) return;
        ColaBFS cola; cola.encolar(inicio->origen); inicio->origen->visitado = true;
        if (!silencioso) std::cout << "\n--- BFS ---" << std::endl;

        while(!cola.estaVacia()) {
            Estudiante* actual = cola.desencolar();
            if (!silencioso) std::cout << actual->student_id << " - " << actual->full_name << std::endl;
            Vertice* vActual = buscarVertice(actual->student_id);
            Arco* ady = vActual->adyacentes;
            while(ady) {
                if(!ady->destino->visitado) {
                    ady->destino->visitado = true; cola.encolar(ady->destino);
                }
                ady = ady->siguiente;
            }
        }
    }

    void DFS(int start_id, bool silencioso = false) {
        Vertice* inicio = buscarVertice(start_id);
        if(!inicio) return;
        PilaDFS pila; pila.apilar(inicio->origen);
        if (!silencioso) std::cout << "\n--- DFS ---" << std::endl;

        while(!pila.estaVacia()) {
            Estudiante* actual = pila.desapilar();
            if(!actual->visitado) {
                actual->visitado = true;
                if (!silencioso) std::cout << actual->student_id << " - " << actual->full_name << std::endl;
                Vertice* vActual = buscarVertice(actual->student_id);
                Arco* ady = vActual->adyacentes;
                while(ady) {
                    if(!ady->destino->visitado) pila.apilar(ady->destino);
                    ady = ady->siguiente;
                }
            }
        }
    }

    bool eliminarVertice(int id) {
        Vertice* actual = primer_vertice; Vertice* anterior = nullptr; Vertice* objetivo = nullptr;
        while (actual) {
            if (actual->origen->student_id == id) {
                objetivo = actual;
                if (anterior) anterior->siguiente = actual->siguiente; else primer_vertice = actual->siguiente;
                break;
            }
            anterior = actual; actual = actual->siguiente;
        }
        if (!objetivo) return false;

        Vertice* tempV = primer_vertice; int aristas_eliminadas = 0;
        while (tempV) {
            Arco* arcAct = tempV->adyacentes; Arco* arcAnt = nullptr;
            while (arcAct) {
                if (arcAct->destino->student_id == id) {
                    if (arcAnt) arcAnt->siguiente = arcAct->siguiente; else tempV->adyacentes = arcAct->siguiente;
                    Arco* aBorrar = arcAct; arcAct = arcAct->siguiente; delete aBorrar; aristas_eliminadas++;
                } else { arcAnt = arcAct; arcAct = arcAct->siguiente; }
            }
            tempV = tempV->siguiente;
        }
        Arco* arcObj = objetivo->adyacentes;
        while (arcObj) { Arco* temp = arcObj; arcObj = arcObj->siguiente; delete temp; }
        delete objetivo; total_conexiones -= aristas_eliminadas; return true;
    }

    int getTotalConexiones() const { return total_conexiones; }
};

#endif