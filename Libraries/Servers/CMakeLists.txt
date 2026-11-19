set(LIB_TARGET_NAME Servers)
COMPONENTS_CCR_CONFIGURE_LIBRARY(true ${LIB_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    boost_serialization
)
COMPONENTS_CCR_SET_LIBRARY_VERSION(${LIB_TARGET_NAME} "1.0.0" true)

COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} ExtraClasses)

set(ROD_LIBRARIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries)
COMPONENTS_CCR_LINK_LIBRARY(Servers Exchange ${ROD_LIBRARIES_ROOT}/Exchange)
COMPONENTS_CCR_LINK_LIBRARY(Servers DataObjects ${ROD_LIBRARIES_ROOT}/DataObjects)

if (${ROD_BUILD_SERVER})
    target_link_libraries(${LIB_TARGET_NAME} drogon)
else()
    COMPONENTS_REMOVE_SOURCES(Servers
            "${CMAKE_CURRENT_SOURCE_DIR}/src/controllerbase.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/controllerbase.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/database/recordmanager.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/database/recordmanager.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/database/recordobjects.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/database/recordobjects.cpp"
        )
endif()