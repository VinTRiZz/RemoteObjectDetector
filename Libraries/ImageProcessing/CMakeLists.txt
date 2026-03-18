COMPONENTS_CCR_FIND_LIBRARY(OpenCV)

COMPONENTS_CCR_CONFIGURE_LIBRARY(ImageProcessing ${CMAKE_CURRENT_LIST_DIR}
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

# GStreamer linking
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GST_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_check_modules(GST_VIDEO REQUIRED IMPORTED_TARGET gstreamer-video-1.0)

target_link_libraries(ImageProcessing PRIVATE
    PkgConfig::GSTREAMER
    PkgConfig::GST_APP
    PkgConfig::GST_VIDEO
)

COMPONENTS_LINK_COMPONENT(ImageProcessing Logger)

COMPONENTS_CCR_LINK_LIBRARY(ImageProcessing DataObjects ${CMAKE_CURRENT_LIST_DIR}/../DataObjects)
COMPONENTS_CCR_LINK_LIBRARY(ImageProcessing TBB::tbb ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty/oneTBB)

file(GLOB TEST_SOURCES "tests/*.cpp")
COMPONENTS_CCR_ADD_TEST(ImageProcessing ${TEST_SOURCES})
