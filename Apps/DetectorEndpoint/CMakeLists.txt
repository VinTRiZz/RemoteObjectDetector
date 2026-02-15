if (NOT ROD_BUILD_DETECTOR)
    message(STATUS "Server build disabled")
    return()
endif()

COMPONENTS_CCR_CONFIGURE_APP(ROD-DetectorEndpoint ${CMAKE_CURRENT_LIST_DIR}
    boost_system
    boost_program_options
)

COMPONENTS_LINK_COMPONENT(ROD-DetectorEndpoint Logger)
COMPONENTS_LINK_COMPONENT(ROD-DetectorEndpoint Common)
