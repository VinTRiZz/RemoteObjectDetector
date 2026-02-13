# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Library searches
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql Network)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql Network)


set(SERVER_TARGET_NAME ServerApp.bin)

# Project files
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")


# Library searches
find_package(boost_system REQUIRED)
find_package(boost_program_options REQUIRED)
find_package(OpenCV REQUIRED)

# Target
add_executable( ${SERVER_TARGET_NAME} )
target_sources(${SERVER_TARGET_NAME} PRIVATE ${HEADERS} ${SOURCES})
target_include_directories(${SERVER_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

# Target linking
target_link_libraries(
    ${SERVER_TARGET_NAME}
    PRIVATE

    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network

    boost_system
    boost_serialization

    # Other libs
    pthread
    crypto
    xcb

    # C++ 14 troubles
    stdc++fs

    # OpenCV
    opencv_videoio
    opencv_imgcodecs
    opencv_video
    opencv_photo
    opencv_imgproc
    opencv_flann
    opencv_core
    opencv_highgui
    opencv_objdetect
    opencv_flann
    opencv_features2d
)

# Build flag configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${SERVER_TARGET_NAME} PRIVATE DEBUG_MODE)
    target_compile_options(${SERVER_TARGET_NAME} PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${SERVER_TARGET_NAME} PRIVATE NDEBUG)
    target_compile_options(${SERVER_TARGET_NAME} PRIVATE -O3)
endif()

# Build suffixes
if (NOT RELEASE)
    set_target_properties(
        ${SERVER_TARGET_NAME}
        PROPERTIES
        SUFFIX ".DEBUG"
    )
else()
    set_target_properties(
        ${SERVER_TARGET_NAME}
        PROPERTIES
        SUFFIX ".bin"
    )
endif()
