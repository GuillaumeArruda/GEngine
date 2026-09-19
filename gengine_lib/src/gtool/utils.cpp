#include "stdafx.h"

#include "gtool/utils.h"

#include "gcore/resource_library.h"

#include "gserializer/serializers/json_serializer.h"

namespace gtool 
{
    std::vector<resource_info> gtool::get_resource_infos(gcore::resource_library& library)
    {
        std::vector<resource_info> return_value;
        auto& uuid_to_files = library.get_uuid_to_resource_files();
        for (auto& uuid_to_file : uuid_to_files)
        {
            auto& res_info = return_value.emplace_back();
            res_info.m_uuid = uuid_to_file.first;

            gserializer::json_read_serializer json_read(uuid_to_file.second.string().c_str());
            json_read.open_scope("resource");
            json_read.process("type_name", res_info.m_resource_type);
            json_read.open_scope("data");
            json_read.process("name", res_info.m_name);
            json_read.close_scope("data");
            json_read.close_scope("resource");
        }
        return return_value;
    }
}

