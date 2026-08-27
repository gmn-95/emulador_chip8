# Arquivo de configuração do projeto

cmake_minimum_required(VERSION 4.3)
project(chip8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(chip8 WIN32 main.cpp)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)