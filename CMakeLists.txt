# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)

# Setup output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)

# Build variables
file(GLOB HEADERS "src/*.hpp" "src/*/*.hpp")
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Setup build target
add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# Search for packages
find_package(OpenCV REQUIRED)

# For easier develop
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Link libraries to target
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    
    # Other libs
    -lpthread
    -lcrypto
    
    # OpenCV
    -lopencv_core
    -lopencv_imgproc
    -lopencv_highgui
    -lopencv_photo
)
