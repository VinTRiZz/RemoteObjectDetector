# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)


# Build variables
set(BIN_PATH bin)
file(GLOB HEADERS *.h inc/*.h)
file(GLOB SOURCES *.cpp src/*.cpp)

# Build output
add_executable(${BIN_PATH}/TargetServer 
        ${SOURCES}
        ${HEADERS}
)
