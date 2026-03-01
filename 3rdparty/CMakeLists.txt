add_subdirectory(websocketpp)

if (NOT ROD_BUILD_SERVER)
    message(STATUS "Server build disabled, Drogon ignored")
    return()
endif()
add_subdirectory(drogon)
