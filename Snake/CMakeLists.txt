cmake_minimum_required(VERSION 3.5)

project(Snake LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Werror -Wfatal-errors -Wall -Wextra -g)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(Snake
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/snake.cpp
    resource/snake.qrc
)

target_link_libraries(Snake PRIVATE Qt5::Widgets)
