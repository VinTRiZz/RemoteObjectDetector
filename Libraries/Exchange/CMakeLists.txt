set(LIB_TARGET_NAME Exchange)
COMPONENTS_CCR_CONFIGURE_LIBRARY(true ${LIB_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}
    bitsery_headers # Serialization
)
COMPONENTS_CCR_SET_LIBRARY_VERSION(${LIB_TARGET_NAME} "1.0.0" true)

COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} ExtraClasses)

COMPONENTS_CCR_LINK_LIBRARY(Exchange ImageProcessing ${CMAKE_CURRENT_LIST_DIR}/../ImageProcessing)

FILE(GLOB EXTRA_HEADERS "${CMAKE_CURRENT_LIST_DIR}/tests/extra/*")
FILE(GLOB TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/tests/*.cpp")
COMPONENTS_CCR_ADD_TEST(Exchange ${EXTRA_HEADERS} ${TEST_SOURCES})

target_precompile_headers(Exchange PUBLIC
    "src/events.hpp"
    "src/httpconstants.hpp"
    "src/imagepacket.hpp"
    "src/sendableimage.hpp"
)
