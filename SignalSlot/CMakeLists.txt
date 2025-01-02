cmake_minimum_required(VERSION 3.5)

project(SignalSlot LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Werror -Wfatal-errors -Wall -Wextra)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(SignalSlot
    main.cpp
    Jerry.h
    Tom.h
)

target_link_libraries(SignalSlot PRIVATE Qt5::Widgets)
