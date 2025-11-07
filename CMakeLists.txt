cmake_minimum_required(VERSION 3.10)
project(KnightTour)

set(CMAKE_CXX_STANDARD 17)

# Добавляем текущую директорию в пути включения
include_directories(include)

add_executable(knight_tour
    src/main.cpp
    src/knight_basic.cpp
    src/knight_advanced.cpp
)