if (NOT ROD_BUILD_DETECTOR)
    message(STATUS "Server build disabled")
    return()
endif()

set(ENDPOINT_TARGET_NAME ROD-DetectorEndpoint)

COMPONENTS_CCR_CONFIGURE_APP(${ENDPOINT_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    boost_system
    boost_program_options
    crypto
    ssl
)

COMPONENTS_LINK_COMPONENT(${ENDPOINT_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${ENDPOINT_TARGET_NAME} Common)
