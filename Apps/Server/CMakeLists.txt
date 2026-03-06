if (NOT ROD_BUILD_SERVER)
    message(STATUS "Server build disabled")
    return()
endif()

set(SERVER_TARGET_NAME ROD-Server)

COMPONENTS_CCR_CONFIGURE_APP(${SERVER_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    ssl
    crypto
    drogon
    boost_system
    boost_program_options

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

COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Common)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Database)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Encryption)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Filework)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} SystemProcessing)
COMPONENTS_LINK_COMPONENT(${SERVER_TARGET_NAME} Network)

set(ROD_LIBRARIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries)
COMPONENTS_CCR_LINK_LIBRARY(${SERVER_TARGET_NAME} DataObjects ${ROD_LIBRARIES_ROOT}/DataObjects)
COMPONENTS_CCR_LINK_LIBRARY(${SERVER_TARGET_NAME} ImageProcessing ${ROD_LIBRARIES_ROOT}/ImageProcessing)
COMPONENTS_CCR_LINK_LIBRARY(${SERVER_TARGET_NAME} Protocol ${ROD_LIBRARIES_ROOT}/Protocol)

if (websocketpp_SOURCE_DIR)
    target_include_directories(${SERVER_TARGET_NAME} PRIVATE ${websocketpp_SOURCE_DIR})
endif()
