#ifndef ESTUDIANTE_H
#define ESTUDIANTE_H

#include <string>
#include <iostream>

// Estructura para la Lista Enlazada de Proyectos
struct Proyecto {
    int project_id;
    std::string title;
    std::string description;
    int completion_year;
    Proyecto* next;

    Proyecto(int id, std::string t, std::string d, int year)
        : project_id(id), title(t), description(d), completion_year(year), next(nullptr) {}
};

// Entidad Central
struct Estudiante {
    int student_id;
    std::string full_name;
    std::string career;
    int semester;
    float gpa;
    int skill_score;
    bool visitado;

    // Puntero al inicio de su lista de proyectos
    Proyecto* proyectos_head;

    Estudiante(int id, std::string name, std::string car, int sem, float _gpa, int score)
        : student_id(id), full_name(name), career(car), semester(sem), gpa(_gpa), skill_score(score), visitado(false), proyectos_head(nullptr) {}

    // Destructor para evitar fugas de memoria con la lista de proyectos
    ~Estudiante() {
        Proyecto* actual = proyectos_head;
        while (actual != nullptr) {
            Proyecto* temp = actual;
            actual = actual->next;
            delete temp;
        }
    }
};

#endif