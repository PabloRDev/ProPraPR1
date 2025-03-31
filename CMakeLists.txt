cmake_minimum_required(VERSION 3.29)  # You can change this based on your setup
project(UOC20242 C)

set(CMAKE_C_STANDARD 11)

# Buscar todas las carpetas 'include' de forma recursiva
file(GLOB_RECURSE INCLUDE_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/**/include")

# Asegurarse de que tenemos algo en la lista
message(STATUS "Include directories found: ${INCLUDE_DIRS}")

# Añadir las carpetas encontradas como directorios de inclusión
include_directories(${INCLUDE_DIRS})

# Asegúrate de que los archivos fuente están correctamente listados
add_executable(UOC20242
        src/main.c
        UOCPlay/src/api.c
        UOCPlay/src/csv.c
        UOCPlay/src/date.c
        UOCPlay/src/film.c
        UOCPlay/src/person.c
        UOCPlay/src/subscription.c
        test/src/test.c
        test/src/test_pr1.c
        test/src/test_suite.c)
