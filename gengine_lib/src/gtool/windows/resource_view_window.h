#pragma once

#include "gtool/window.h"
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

    struct resource_view_window : window
    {
        void update(gcore::world& world, window_manager& manager) override;
        bool& get_should_display() override { return m_display; }
        const char* get_name() const override { return "Resource Viewer"; }
    private:
        bool m_display = true;
        resource_viewer_widget m_resource_viewer_widget;
        create_resource_widget m_create_resource_widget;
    };
}