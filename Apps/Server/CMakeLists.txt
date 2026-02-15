if (NOT ROD_BUILD_SERVER)
    message(STATUS "Server build disabled")
    return()
endif()

COMPONENTS_CCR_CONFIGURE_APP(ROD-Server ${CMAKE_CURRENT_LIST_DIR}
    ssl
    crypto
    drogon
    boost_system
    boost_program_options
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(DEBUG_BUILD_MODE)
endif()

COMPONENTS_LINK_COMPONENT(ROD-Server Logger)
COMPONENTS_LINK_COMPONENT(ROD-Server Common)
COMPONENTS_LINK_COMPONENT(ROD-Server Database)
COMPONENTS_LINK_COMPONENT(ROD-Server Encryption)
COMPONENTS_LINK_COMPONENT(ROD-Server SystemProcessing)
