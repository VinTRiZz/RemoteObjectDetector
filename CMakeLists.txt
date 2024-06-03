# Build cmake setting
cmake_minimum_required(VERSION 3.7)
project(ObjectSearcher LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

# Setup output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Network)
find_package(Qt5 REQUIRED COMPONENTS Network)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Network)

# Link libraries to target
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    
    # Other libs
    -lpthread
    -lcrypto

    # C++ 14 troubles
    -lstdc++fs
    
    # OpenCV
#    -lopencv_videoio
#    -lopencv_imgcodecs
#    -lopencv_video
#    -lopencv_photo
#    -lopencv_imgproc
#    -lopencv_flann
#    -lopencv_core
#    -lopencv_highgui
-lopencv_dnn -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_videoio -lopencv_imgcodecs -lopencv_features2d -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core
)

# For easier develop
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
