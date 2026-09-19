#pragma once

#include <string>
#include <vector>

#include "gtl/uuid.h"

namespace gcore 
{
    struct resource_library;
}

namespace gtool 
{
    struct resource_info
    {
        gtl::uuid m_uuid;
        std::string m_name;
        std::string m_resource_type;
    };

    std::vector<resource_info> get_resource_infos(gcore::resource_library& library);
}