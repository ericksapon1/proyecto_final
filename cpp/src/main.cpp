#include <iostream>
#include "SystemManager.h"

void mostrarMenu() {
    std::cout << "\n=== RED SOCIAL ACADEMICA ===" << std::endl;
    std::cout << "\n Erick Rolando Sapon Portela" << std::endl;
    std::cout << "9941-20-11626" << std::endl;
    std::cout << "\n1. Cargar estudiantes desde CSV" << std::endl;
    std::cout << "2. Registrar estudiante manualmente" << std::endl;
    std::cout << "3. Buscar estudiante por ID" << std::endl;
    std::cout << "4. Eliminar estudiante" << std::endl;
    std::cout << "5. Agregar proyecto a estudiante" << std::endl;
    std::cout << "6. Mostrar proyectos de estudiante" << std::endl;
    std::cout << "7. Conectar estudiantes" << std::endl;
    std::cout << "8. Verificar si dos estudiantes estan conectados" << std::endl;
    std::cout << "9. Mostrar conexiones de estudiante" << std::endl;
    std::cout << "10. Recorrer red con BFS" << std::endl;
    std::cout << "11. Recorrer red con DFS" << std::endl;
    std::cout << "12. Mostrar ranking academico (AVL)" << std::endl;
    std::cout << "13. Mostrar estadisticas de estructuras" << std::endl;
    std::cout << "14. Generar benchmark C++" << std::endl;
    std::cout << "15. Salir" << std::endl;
    std::cout << "Seleccione una opcion: ";
}

int main() {
    SystemManager sistema; int opcion;

    do {
        mostrarMenu();
        if (!(std::cin >> opcion)) { std::cin.clear(); std::cin.ignore(10000, '\n'); opcion = 0; }

        switch (opcion) {
            case 1: sistema.cargarDesdeCSV("data/estudiantes.csv"); break;
            case 2: {
                int id, sem, score; float gpa; std::string nombre, carrera;
                std::cout << "ID: "; std::cin >> id; std::cin.ignore();
                std::cout << "Nombre completo: "; std::getline(std::cin, nombre);
                std::cout << "Carrera: "; std::getline(std::cin, carrera);
                std::cout << "Semestre: "; std::cin >> sem;
                std::cout << "GPA: "; std::cin >> gpa;
                std::cout << "Score (Ranking): "; std::cin >> score;
                sistema.registrarEstudianteManualmente(id, nombre, carrera, sem, gpa, score);
                break;
            }
            case 3: {
                int id; std::cout << "Ingrese ID a buscar: "; std::cin >> id;
                sistema.buscarEstudiante(id); break;
            }
            case 4: {
                int id; std::cout << "Ingrese ID del estudiante a eliminar: "; std::cin >> id;
                sistema.eliminarEstudiante(id); break;
            }
            case 5: {
                int s_id, p_id, anio; std::string titulo, desc;
                std::cout << "ID estudiante: "; std::cin >> s_id;
                std::cout << "ID proyecto: "; std::cin >> p_id; std::cin.ignore();
                std::cout << "Titulo: "; std::getline(std::cin, titulo);
                std::cout << "Descripcion: "; std::getline(std::cin, desc);
                std::cout << "Anio: "; std::cin >> anio;
                sistema.agregarProyecto(s_id, p_id, titulo, desc, anio); break;
            }
            case 6: {
                int s_id; std::cout << "ID estudiante: "; std::cin >> s_id;
                sistema.mostrarProyectos(s_id); break;
            }
            case 7: {
                int sub; std::cout << "\n1. Conectar\n2. Deshacer ultima (Undo)\nElige: "; std::cin >> sub;
                if (sub == 1) {
                    int id1, id2; std::cout << "ID 1: "; std::cin >> id1; std::cout << "ID 2: "; std::cin >> id2;
                    sistema.conectarEstudiantes(id1, id2);
                } else if (sub == 2) { sistema.deshacerUltimaConexion(); }
                break;
            }
            case 8: {
                int id1, id2; std::cout << "ID 1: "; std::cin >> id1; std::cout << "ID 2: "; std::cin >> id2;
                sistema.verificarConexion(id1, id2); break;
            }
            case 9: { int id; std::cout << "ID estudiante: "; std::cin >> id; sistema.mostrarConexiones(id); break; }
            case 10: { int id; std::cout << "ID inicial BFS: "; std::cin >> id; sistema.recorrerBFS(id); break; }
            case 11: { int id; std::cout << "ID inicial DFS: "; std::cin >> id; sistema.recorrerDFS(id); break; }
            case 12: sistema.mostrarRanking(); break;
            case 13: sistema.mostrarEstadisticas(); break;
            case 14: sistema.generarBenchmarkC("data/estudiantes.csv"); break;
            case 15: std::cout << "Saliendo..." << std::endl; break;
            default: std::cout << "Opcion invalida." << std::endl; break;
        }
    } while (opcion != 15);

    return 0;
}