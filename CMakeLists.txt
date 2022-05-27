cmake_minimum_required(VERSION 3.20)
project(sortVisualiser)

set(CMAKE_CXX_STANDARD 14)

add_executable(sortVisualiser main.cpp)

set(SFML_STATIC_LIBRARIES true)


set(SFML_DIR ${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/SFML/include)

target_link_libraries(sortVisualiser sfml-system sfml-window sfml-graphics sfml-audio)