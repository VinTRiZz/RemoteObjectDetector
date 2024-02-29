# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Build variables
file(GLOB HEADERS src/*.h src/study/*.h)
file(GLOB SOURCES src/*.cpp src/study/*.cpp)

# Build output
add_executable(ObjectDetector 
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(ObjectDetector -lcrypto)
target_link_libraries(ObjectDetector -lpthread)

find_package(OpenCV REQUIRED)
target_link_libraries(ObjectDetector -lopencv_core)
target_link_libraries(ObjectDetector -lopencv_imgproc)
target_link_libraries(ObjectDetector -lopencv_highgui)
target_link_libraries(ObjectDetector -lopencv_photo)
