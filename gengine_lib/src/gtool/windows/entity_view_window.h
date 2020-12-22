#pragma once

#include "gcore/component.h"

#include "gtool/window.h"
#include "gtool/common_widgets.h"

#include <imgui/imgui.h>

namespace gcore
{
    struct entity_registry;
}

namespace gtool
{
    struct create_entity_widget
    {
        void update(gcore::world& registry);
    private:
        factory_combo_box<gcore::component> m_create_component_combo;
        std::vector<std::unique_ptr<gcore::component>> m_components;
    };

    struct save_load_registry_widget
    {
        void update(gcore::world& world);
    private:
        std::string m_filepath;
    };

    struct selected_entity_widget
    {
        void update(gcore::world& world);
        std::string m_uuid;
    private:
        factory_combo_box<gcore::component> m_component_combo_box;
    };

    struct entity_browser_widget
    {
        enum column_id
        {
            name,
            uuid
        };
        void update(gcore::world& world, selected_entity_widget& selected_widget);
        ImGuiTextFilter m_filter;
        column_id m_filter_by;
    };

    struct entity_view_window : window
    {
        void update(gcore::world& world, window_manager& manager);
        bool& get_should_display() override { return m_display; };
        const char* get_name() const override{ return "Entity Viewer"; }
    private:
        bool m_display = true;
        create_entity_widget m_create_entity_widget;
        save_load_registry_widget m_save_load_registry_widget;
        selected_entity_widget m_selected_entity_widget;
        entity_browser_widget m_entity_browser_widget;
    };
}