#pragma once

#include <Components/Common/DirectoryManager.h>

namespace ServerCommon
{

const int DIRTYPE_SOFT_VERSIONS = Common::DirectoryManager::DirectoryType::UserDefined + 1;

using id_t = unsigned long long;

}
