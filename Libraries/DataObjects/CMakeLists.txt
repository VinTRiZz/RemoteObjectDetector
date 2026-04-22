COMPONENTS_CCR_CONFIGURE_LIBRARY(DataObjects ${CMAKE_CURRENT_LIST_DIR})

COMPONENTS_LINK_COMPONENT(DataObjects Logger)
COMPONENTS_LINK_COMPONENT(DataObjects ExtraClasses)
COMPONENTS_LINK_COMPONENT(DataObjects Encryption)

target_precompile_headers(DataObjects PUBLIC
    "src/detectionobject.hpp"
    "src/detectorconfiguration.hpp"
    "src/serializableobject.hpp"

    <Components/ExtraClasses/JOptional.h>
)