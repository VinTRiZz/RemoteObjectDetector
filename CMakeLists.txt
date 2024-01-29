# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)


# Build variables
set(BIN_PATH bin)
set(HEADERS main.cpp)

# Build output
add_executable(${BIN_PATH}/TargetServer ${HEADERS})
