#pragma once

#include "gtl/uuid.h"

namespace gcore
{
    using entity = gtl::uuid;
    using component_id = std::type_index;
    using view_id = std::type_index;
}