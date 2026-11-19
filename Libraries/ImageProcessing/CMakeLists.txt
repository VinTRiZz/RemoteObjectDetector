find_package(OpenCV REQUIRED)

set(LIB_TARGET_NAME ImageProcessing)
COMPONENTS_CCR_CONFIGURE_LIBRARY(true ${LIB_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
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

    # Image hash calculation
    xxhash
)
COMPONENTS_CCR_SET_LIBRARY_VERSION(${LIB_TARGET_NAME} "1.0.0" true)

COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} Logger)

file(GLOB TEST_SOURCES "tests/*.cpp")
COMPONENTS_CCR_ADD_TEST(ImageProcessing ${TEST_SOURCES})

target_precompile_headers(ImageProcessing PUBLIC
    "src/utility.hpp"
    "src/common.hpp"
)

set(ROD_LIBRARIES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries)
COMPONENTS_CCR_LINK_LIBRARY(${LIB_TARGET_NAME} DataObjects ${ROD_LIBRARIES_ROOT}/DataObjects)