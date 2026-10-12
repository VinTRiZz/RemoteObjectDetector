#pragma once

#include <stdint.h>
#include <Components/ExtraClasses/Containers/JOptional.h>

namespace DataObjects {

using id_t = ExtraClasses::JOptional<int64_t>;
const id_t NULL_ID = ExtraClasses::JOptional<int64_t>();

} // namespace DataObjects
