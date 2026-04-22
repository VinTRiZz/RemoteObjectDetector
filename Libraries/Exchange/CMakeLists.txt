COMPONENTS_CCR_CONFIGURE_LIBRARY(Exchange ${CMAKE_CURRENT_LIST_DIR}
    boost_serialization
)

COMPONENTS_LINK_COMPONENT(Exchange Logger)
COMPONENTS_LINK_COMPONENT(Exchange ExtraClasses)

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
