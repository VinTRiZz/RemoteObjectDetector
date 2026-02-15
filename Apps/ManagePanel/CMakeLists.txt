if (NOT ROD_BUILD_MANAGE_PANEL)
    message(STATUS "Manage panel build disabled")
    return()
endif()

COMPONENTS_CCR_ADD_QT(5 Core Widgets Network)

# TODO: How to move it into CMake component????
# Qt configuration
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

COMPONENTS_CCR_CONFIGURE_APP(ROD-ManagePanel ${CMAKE_CURRENT_LIST_DIR} Qt5::Widgets)

COMPONENTS_LINK_COMPONENT(ROD-ManagePanel Logger)
COMPONENTS_LINK_COMPONENT(ROD-ManagePanel Common)
