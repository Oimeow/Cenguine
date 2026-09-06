cmake_minimum_required(VERSION 3.20)

project(CenguineBinary)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(CenguineBinary
    src/main.cpp
    src/display.cpp
    src/components.cpp
    src/rendering.cpp
)

target_include_directories(CenguineBinary PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CenguineBinary PRIVATE
    raylib
    glm::glm
)