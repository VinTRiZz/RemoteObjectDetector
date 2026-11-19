set(LIB_TARGET_NAME DataObjects)
COMPONENTS_CCR_CONFIGURE_LIBRARY(true ${LIB_TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR})
COMPONENTS_CCR_SET_LIBRARY_VERSION(${LIB_TARGET_NAME} "1.0.0" true)

COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} Logger)
COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} ExtraClasses)
COMPONENTS_LINK_COMPONENT(${LIB_TARGET_NAME} Encryption)

target_precompile_headers(${LIB_TARGET_NAME} PUBLIC
    "src/detectionobject.hpp"
    "src/detectorconfiguration.hpp"
    "src/serializableobject.hpp"
)
