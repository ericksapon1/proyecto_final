cmake_minimum_required(VERSION 3.20)
project(Proyecto_Final)

set(CMAKE_CXX_STANDARD 17)

# Incluye tanto la raíz del proyecto como la carpeta 'include' para buscar los .h
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/Include)

# Atrapa los .cpp sin importar si están en la raíz o en 'src/'
file(GLOB SOURCES "*.cpp" "src/*.cpp")

add_executable(Proyecto_Final ${SOURCES})

# Copia la carpeta data/ (donde debe estar tu CSV) al entorno de compilación
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})