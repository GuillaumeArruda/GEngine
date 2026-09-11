#pragma once

#include "gcore/script/script.h"

namespace grender 
{
    struct graphic_script_frame_input_data
    {
        glm::mat4 m_transform;
        glm::mat4 m_projection;
        glm::mat4 m_view_matrix;
    };

    struct graphic_script : gcore::script
    {
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();

        ~graphic_script();
    };
}


