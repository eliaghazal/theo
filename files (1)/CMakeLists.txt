cmake_minimum_required(VERSION 3.16)

project(DFAMinimizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/GraphScene.cpp
    src/GraphScene.h
    src/items/StateItem.cpp
    src/items/StateItem.h
    src/items/TransitionItem.cpp
    src/items/TransitionItem.h
    src/model/DFA.cpp
    src/model/DFA.h
    src/algorithms/Hopcroft.cpp
    src/algorithms/Hopcroft.h
    src/io/DFAJson.cpp
    src/io/DFAJson.h
)

add_executable(DFAMinimizer ${SOURCES})

target_link_libraries(DFAMinimizer PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core)

if(WIN32)
    set_property(TARGET DFAMinimizer PROPERTY WIN32_EXECUTABLE ON)
endif()