#pragma once

#include "gcore/system.h"
#include "gcore/component.h"

namespace gcore
{
    struct entity_registry;
}

namespace gtool
{
    struct component_type_combo_box
    {
        component_type_combo_box();
        void draw(const char* name);
        const char* get_selected_component_type() const { return m_current_component_type; }
    private:
        std::vector<std::string> m_component_type_names;
        const char* m_current_component_type = nullptr;
    };

    struct create_entity_widget
    {
        void update(gcore::entity_registry& registry);
    private:
        component_type_combo_box m_create_component_combo;
        std::vector<std::unique_ptr<gcore::component>> m_components;
    };

    struct save_load_registry_widget
    {
        void update(gcore::entity_registry& registry);
    private:
        std::string m_filepath;
    };

    struct selected_entity_widget
    {
        void update(gcore::entity_registry& registry);
    private:
        component_type_combo_box m_component_combo_box;
        std::string m_uuid;
    };

    struct create_entity_system : gcore::system
    {
        void update(gcore::entity_registry& registry);
    private:
        bool m_display = true;
        create_entity_widget create_entity_widget;
        save_load_registry_widget save_load_registry_widget;
        selected_entity_widget selected_entity_widget;
    };
}