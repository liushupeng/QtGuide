cmake_minimum_required(VERSION 3.5)

project(02QtSnake LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Werror -Wfatal-errors -Wall -Wextra)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(02QtSnake
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/snake.cpp
    resource/snake.qrc
)

target_link_libraries(02QtSnake PRIVATE Qt5::Widgets)
