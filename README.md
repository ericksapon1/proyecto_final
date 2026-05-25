Social Network Academic Manager (SNAM)
Proyecto final del curso de Programación III, enfocado en la gestión eficiente de una red social académica mediante estructuras de datos implementadas manualmente desde cero.

Descripción del Proyecto
El sistema gestiona una red de estudiantes, sus carreras, puntajes (skill scores) y conexiones sociales. Se ha priorizado la optimización de memoria y la complejidad temporal, utilizando estructuras de datos fundamentales en C++ y comparando su rendimiento contra las colecciones nativas de Java.

Arquitectura Técnica
El proyecto está dividido en módulos para asegurar escalabilidad y modularidad:

src/: Implementación de la lógica del sistema (SystemManager).

Include/: Cabeceras de estructuras (TablaHash, ArbolAVL, Grafo).

java/: Módulo de benchmark comparativo.

data/: Dataset principal (estudiantes.csv).

