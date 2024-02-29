# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# Build variables
file(GLOB HEADERS src/*.h)
file(GLOB SOURCES src/*.cpp)

# include_directories(/home/lazarev_as/Projects/Qt/UtilityLibrary/include)

# Build output
add_executable(ObjectDetector 
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(ObjectDetector -lcrypto)
target_link_libraries(ObjectDetector -lpthread)
# target_link_libraries(ObjectDetector -Llibs/UtilityLibrary)
