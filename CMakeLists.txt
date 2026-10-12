# CMake required
cmake_minimum_required(VERSION 3.7)

# C++ configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    RemoteObjectDetector
    VERSION 1.0.0
    LANGUAGES C CXX
)
enable_testing() # Required for ctest (CI/CD)


# TODO: How to move it into CMake component????
# Installation
set(INSTALL_DESTINATION_PREFIX
    "${CMAKE_CURRENT_LIST_DIR}/BIN"
)
set(CMAKE_INSTALL_PREFIX
    "${INSTALL_DESTINATION_PREFIX}"
)
set_property(GLOBAL PROPERTY COMPONENTS_INSTALL_PATH ${INSTALL_DESTINATION_PREFIX})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD_MODE)
endif()

add_subdirectory(ComponentsSystem)

COMPONENTS_CCR_ENABLE_LTO()

# Build options (for making on target device)
option (ROD_BUILD_MANAGE_PANEL ON)
option (ROD_BUILD_DETECTOR ON)
option (ROD_BUILD_SERVER ON)

set( ROD_BUILD_DETECTOR ON CACHE BOOL "Default detector building enable" FORCE)

set( ROD_DATA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/DATA" )

# Conan 2.x dependency bootstrap (defined in Components-CMake).
# It is a no-op when Conan / the conanfile is missing
# or when an external tool (Qt Creator) already configured Conan.
COMPONENTS_CCR_CONAN_SETUP(
    CONANFILE "${CMAKE_CURRENT_SOURCE_DIR}/conanfile.txt"
    BUILD     "missing"
)

# Boost: resolved through Conan when it is active, otherwise from the system
COMPONENTS_CCR_CONAN_FIND_LIBRARY(boost_system)
COMPONENTS_CCR_CONAN_FIND_LIBRARY(boost_thread)
COMPONENTS_CCR_CONAN_FIND_LIBRARY(boost_program_options)
COMPONENTS_CCR_DEFINE_PROJECT(${PROJECT_NAME} ${PROJECT_VERSION} 3rdparty Libraries Apps)
