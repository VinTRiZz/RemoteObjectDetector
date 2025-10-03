# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(GUI_TARGET_NAME ControlGUI.bin)

# Project files
file(GLOB_RECURSE HEADERS "src/*.hpp" "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE UIFILES "src/*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")
source_group("Ui Files" FILES ${UIFILES})


# Library searches
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql Network)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql Network)
find_package(OpenCV REQUIRED)

# Target
add_executable( ${GUI_TARGET_NAME} )
target_sources(${GUI_TARGET_NAME} PRIVATE ${HEADERS} ${SOURCES} ${UIFILES})
target_include_directories(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

# Target linking
target_link_libraries(
    ${GUI_TARGET_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    boost_system
    boost_serialization

    # OpenCV
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
)

# Build flag configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE DEBUG_MODE)
    target_compile_options(${GUI_TARGET_NAME} PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE NDEBUG)
    target_compile_options(${GUI_TARGET_NAME} PRIVATE -O3)
endif()

# Build suffixes
if (NOT RELEASE)
    set_target_properties(
        ${GUI_TARGET_NAME}
        PROPERTIES
        SUFFIX ".DEBUG"
    )
else()
    set_target_properties(
        ${GUI_TARGET_NAME}
        PROPERTIES
        SUFFIX ".bin"
    )
endif()
