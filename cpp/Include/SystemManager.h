#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include "Estudiante.h"
#include "TablaHash.h"
#include "ArbolAVL.h"
#include "Grafo.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>

struct NodoUndo {
    int id1; int id2; NodoUndo* sig;
    NodoUndo(int a, int b) : id1(a), id2(b), sig(nullptr) {}
};

class PilaUndo {
private:
    NodoUndo* tope;
public:
    PilaUndo() : tope(nullptr) {}
    ~PilaUndo() { while (tope) { NodoUndo* temp = tope; tope = tope->sig; delete temp; } }
    void apilar(int id1, int id2) { NodoUndo* nuevo = new NodoUndo(id1, id2); nuevo->sig = tope; tope = nuevo; }
    bool desapilar(int& id1, int& id2) {
        if (!tope) return false;
        id1 = tope->id1; id2 = tope->id2; NodoUndo* temp = tope; tope = tope->sig; delete temp; return true;
    }
};

class SystemManager {
private:
    TablaHash tablaHash;
    ArbolAVL arbolAVL;
    Grafo grafo;
    PilaUndo pilaUndo;
    std::vector<Estudiante*> todos_los_estudiantes;

    void resetearVisitados() { for (auto est : todos_los_estudiantes) est->visitado = false; }

public:
    SystemManager() : tablaHash(15000) {}
    ~SystemManager() { for (auto est : todos_los_estudiantes) delete est; }

    bool cargarDesdeCSV(const std::string& ruta_archivo) {
        auto inicio = std::chrono::high_resolution_clock::now();
        std::ifstream archivo(ruta_archivo);
        if (!archivo.is_open()) return false;
        std::string linea; std::getline(archivo, linea);

        while (std::getline(archivo, linea)) {
            if (linea.empty()) continue;
            std::stringstream ss(linea); std::string item, nombre, carrera; int id, sem, score; float gpa;
            try {
                std::getline(ss, item, ','); id = std::stoi(item); std::getline(ss, nombre, ',');
                std::getline(ss, carrera, ','); std::getline(ss, item, ','); sem = std::stoi(item);
                std::getline(ss, item, ','); gpa = std::stof(item); std::getline(ss, item, ','); score = std::stoi(item);
                Estudiante* nuevoEst = new Estudiante(id, nombre, carrera, sem, gpa, score);
                todos_los_estudiantes.push_back(nuevoEst);
                tablaHash.insertar(nuevoEst); arbolAVL.insertar(nuevoEst); grafo.insertarVertice(nuevoEst);
            } catch (const std::exception& e) {}
        }
        auto fin = std::chrono::high_resolution_clock::now();
        std::cout << "Carga Exitosa en " << std::chrono::duration<double, std::milli>(fin - inicio).count() << " ms\n";
        return true;
    }

    void registrarEstudianteManualmente(int id, const std::string& nombre, const std::string& carrera, int sem, float gpa, int score) {
        if (tablaHash.buscar(id) != nullptr) {
            std::cout << "Error: El ID " << id << " ya existe." << std::endl; return;
        }
        Estudiante* nuevoEst = new Estudiante(id, nombre, carrera, sem, gpa, score);
        todos_los_estudiantes.push_back(nuevoEst);
        tablaHash.insertar(nuevoEst);
        arbolAVL.insertar(nuevoEst);
        grafo.insertarVertice(nuevoEst);
        std::cout << "Estudiante registrado exitosamente." << std::endl;
    }

    void buscarEstudiante(int id) const {
        Estudiante* est = tablaHash.buscar(id);
        if (est) {
            std::cout << "\n--- Datos del Estudiante ---" << std::endl;
            std::cout << "ID: " << est->student_id << " | Nombre: " << est->full_name << std::endl;
            std::cout << "Carrera: " << est->career << " | Semestre: " << est->semester << std::endl;
            std::cout << "GPA: " << est->gpa << " | Score: " << est->skill_score << std::endl;
        } else {
            std::cout << "Estudiante no encontrado." << std::endl;
        }
    }

    bool eliminarEstudiante(int id) {
        Estudiante* est = tablaHash.buscar(id);
        if (!est) return false;
        int score = est->skill_score;
        grafo.eliminarVertice(id); arbolAVL.eliminar(score, id); tablaHash.eliminar(id);
        todos_los_estudiantes.erase(std::remove(todos_los_estudiantes.begin(), todos_los_estudiantes.end(), est), todos_los_estudiantes.end());
        delete est; std::cout << "Estudiante " << id << " eliminado." << std::endl; return true;
    }

    bool agregarProyecto(int student_id, int project_id, const std::string& titulo, const std::string& desc, int anio) {
        Estudiante* est = tablaHash.buscar(student_id);
        if (!est) return false;
        Proyecto* actual = est->proyectos_head;
        while (actual != nullptr) {
            if (actual->project_id == project_id) return false; actual = actual->next;
        }
        Proyecto* nuevoProyecto = new Proyecto(project_id, titulo, desc, anio);
        nuevoProyecto->next = est->proyectos_head; est->proyectos_head = nuevoProyecto;
        return true;
    }

    void mostrarProyectos(int student_id) const {
        Estudiante* est = tablaHash.buscar(student_id);
        if (!est) return;
        Proyecto* actual = est->proyectos_head;
        if (!actual) { std::cout << "Sin proyectos." << std::endl; return; }
        while (actual != nullptr) {
            std::cout << "ID: " << actual->project_id << " | Titulo: " << actual->title << std::endl;
            actual = actual->next;
        }
    }

    void conectarEstudiantes(int id1, int id2) {
        if(grafo.conectar(id1, id2)) { pilaUndo.apilar(id1, id2); std::cout << "Conexion exitosa." << std::endl; }
    }
    void deshacerUltimaConexion() {
        int id1, id2;
        if (pilaUndo.desapilar(id1, id2)) { if (grafo.desconectar(id1, id2)) std::cout << "UNDO exitoso." << std::endl; }
    }
    void verificarConexion(int id1, int id2) const {
        if(grafo.estanConectados(id1, id2)) std::cout << "SI conectados." << std::endl; else std::cout << "NO conectados." << std::endl;
    }

    void mostrarConexiones(int id) const { grafo.mostrarConexiones(id); }
    void recorrerBFS(int id) { resetearVisitados(); grafo.BFS(id, false); }
    void recorrerDFS(int id) { resetearVisitados(); grafo.DFS(id, false); }
    void mostrarRanking() const { arbolAVL.mostrarRanking(); }
    void mostrarEstadisticas() const {
        std::cout << "\n--- ESTADISTICAS ---\nEstudiantes: " << tablaHash.getTotalElementos() << "\nConexiones: " << grafo.getTotalConexiones()
                  << "\nAltura AVL: " << arbolAVL.getAlturaTotal() << "\n--------------------\n";
    }


    void generarBenchmarkC(const std::string& ruta_archivo) {
        std::cout << "Generando benchmark (aislado de la memoria actual)..." << std::endl;

        // 1. Cargar datos a un buffer intermedio para no medir el disco
        std::vector<Estudiante*> buffer;
        std::ifstream archivo(ruta_archivo);
        if (!archivo.is_open()) return;
        std::string linea; std::getline(archivo, linea);
        while (std::getline(archivo, linea)) {
            if (linea.empty()) continue;
            std::stringstream ss(linea); std::string item, nombre, carrera; int id, sem, score; float gpa;
            std::getline(ss, item, ','); id = std::stoi(item); std::getline(ss, nombre, ',');
            std::getline(ss, carrera, ','); std::getline(ss, item, ','); sem = std::stoi(item);
            std::getline(ss, item, ','); gpa = std::stof(item); std::getline(ss, item, ','); score = std::stoi(item);
            buffer.push_back(new Estudiante(id, nombre, carrera, sem, gpa, score));
        }

        int records = buffer.size();
        std::ofstream csv("cpp_results.csv");
        csv << "language,operation,structure,records,time_ms\n";
        csv << std::fixed << std::setprecision(4);

        TablaHash hashBench(records * 2); ArbolAVL avlBench; Grafo grafoBench;

        // Medir Insert HashTable
        auto t1 = std::chrono::high_resolution_clock::now();
        for (auto est : buffer) hashBench.insertar(est);
        auto t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,insert,HashTable," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        // Medir Search HashTable
        t1 = std::chrono::high_resolution_clock::now();
        for (auto est : buffer) hashBench.buscar(est->student_id);
        t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,search,HashTable," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        // Medir Insert AVL
        t1 = std::chrono::high_resolution_clock::now();
        for (auto est : buffer) avlBench.insertar(est);
        t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,insert,AVL," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        // Medir Traversal AVL
        t1 = std::chrono::high_resolution_clock::now();
        avlBench.recorridoSilencioso();
        t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,traversal,AVL," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        // Preparar Grafo para BFS/DFS (crear una cadena de conexiones masiva)
        for (auto est : buffer) grafoBench.insertarVertice(est);
        for (size_t i = 0; i < buffer.size() - 1; ++i) grafoBench.conectar(buffer[i]->student_id, buffer[i+1]->student_id);

        for (auto est : buffer) est->visitado = false;
        t1 = std::chrono::high_resolution_clock::now();
        if(!buffer.empty()) grafoBench.BFS(buffer[0]->student_id, true);
        t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,bfs,Graph," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        for (auto est : buffer) est->visitado = false;
        t1 = std::chrono::high_resolution_clock::now();
        if(!buffer.empty()) grafoBench.DFS(buffer[0]->student_id, true);
        t2 = std::chrono::high_resolution_clock::now();
        csv << "C++,dfs,Graph," << records << "," << std::chrono::duration<double, std::milli>(t2 - t1).count() << "\n";

        csv.close();
        for (auto est : buffer) delete est;
        std::cout << "Archivo cpp_results.csv generado con exito en el directorio de compilacion." << std::endl;
    }
};

#endif // SYSTEMMANAGER_H