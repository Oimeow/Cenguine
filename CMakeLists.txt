cmake_minimum_required(VERSION 3.20)

project(runcenguine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(runcenguine
    src/main.cpp
    src/display.cpp
    src/components.cpp
    src/rendering.cpp
    src/utils.cpp
)

target_include_directories(runcenguine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(runcenguine PRIVATE
    raylib
    glm::glm
)