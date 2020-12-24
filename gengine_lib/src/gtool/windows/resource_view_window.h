#pragma once

#include "gtool/window.h"
#include "gtool/common_widgets.h"

#include "gcore/resource.h"

#include <imgui/imgui.h>

namespace gcore
{
    struct resource;
    struct resource_library;
}

namespace gtool
{
    struct create_resource_widget
    {
        void update();
    private:
        factory_combo_box<gcore::resource> m_resource_type;
        std::string m_folder_path = "data\\";
        std::string m_import_file;
        std::string m_import_name;
        float m_import_scale = 1.f;
    };

    struct resource_viewer_widget
    {
        void update(gcore::resource_library& library);
        std::string m_uuid;
    };

    struct resource_browser_widget
    {
        void update(gcore::resource_library& library, resource_viewer_widget& viewer_widget);
    private:
        enum column_id
        {
            name,
            type,
            uuid
        };
        struct resource_info
        {
            gtl::uuid m_uuid;
            std::string m_name;
            std::string m_resource_type;
        };
        std::vector<resource_info> m_resources_info;
        column_id m_filter_by = column_id::name;
        ImGuiTextFilter m_filter;
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
        resource_browser_widget m_resource_browser;
    };
}