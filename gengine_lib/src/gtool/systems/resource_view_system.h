#pragma once

#include "gcore/system.h"

#include "gtool/common_widgets.h"

#include "gcore/resource.h"

namespace gcore
{
    struct resource;
    struct resource_library;
}

namespace gtool
{
    struct create_resource_widget
    {
        void update(gcore::resource_library& library);
    private:
        factory_combo_box<gcore::resource> m_resource_type;
        std::string m_folder_path;
    };

    struct resource_viewer_widget
    {
        void update(gcore::resource_library& library);
    private:
        std::string m_uuid;
    };

    struct resouce_view_system : gcore::system
    {
        void update(gcore::resource_library& library);
    private:
        resource_viewer_widget m_resource_viewer_widget;
        create_resource_widget m_create_resource_widget;
    };
}