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

COMPONENTS_LINK_COMPONENT(ImageProcessing Logger)
COMPONENTS_LINK_COMPONENT(ImageProcessing Thread)

COMPONENTS_CCR_LINK_LIBRARY(ImageProcessing DataObjects ${CMAKE_CURRENT_LIST_DIR}/../DataObjects)
