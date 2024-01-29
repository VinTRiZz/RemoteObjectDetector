# Build cmake setting
cmake_minimum_required(VERSION 2.8)
project(ObjectDetector)


# Build variables
set(BIN_PATH bin)
file(GLOB HEADERS *.h inc/*.h)
file(GLOB SOURCES *.cpp src/*.cpp)

# include_directories(/home/lazarev_as/Projects/Qt/UtilityLibrary/include)

# Build output
add_executable(${BIN_PATH}/TargetServer 
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(${BIN_PATH}/TargetServer -lcrypto)
# target_link_libraries(${BIN_PATH}/TargetServer -Llibs/UtilityLibrary)
