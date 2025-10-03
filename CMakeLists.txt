# Build cmake setting
cmake_minimum_required(VERSION 3.7)
project(ObjectSearcher LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# Setup output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build binary directory
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN
)

set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/BIN
)

set(
    CMAKE_CACHEFILE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/build
)

add_subdirectory(ServerApp)
add_subdirectory(ControlGUI)
