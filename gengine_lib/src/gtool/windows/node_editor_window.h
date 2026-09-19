#pragma once

#include "gtool/window.h"

#include "ax_NodeEditor/imgui_node_editor.h"

#include "gcore/script/script.h"
#include "gtool/utils.h"

namespace gcore
{
    struct resource_library;
}

namespace gtool
{
    struct node_viewer_widget
    {
        void update(gcore::script_descriptor& script_descriptor, gcore::resource_library& library);
        gcore::node_id_t m_node_id = 0;
    };

    struct create_node_widget
    {
        std::unique_ptr<gcore::node> update();
        bool m_has_just_opened = false;
        ImVec2 m_open_mouse_position;
        ImGuiTextFilter m_filter;
    };

    struct node_editor_window : window
    {
        node_editor_window();
        ~node_editor_window();

        void update(gcore::world& world, window_manager& manager) override;
        const char* get_name() const override { return "Node Editor"; }
        virtual bool& get_should_display() { return m_display; }

    private:
        void open_script(gcore::resource_library& library);
        void save_file();
        void create_script();
        void display_node(gcore::script_descriptor::node_descriptor& node_desc);
        void create_new_elements();
        void delete_elements();
        void create_new_nodes();
        void setup_new_node(std::unique_ptr<gcore::node> new_node);

        node_viewer_widget m_node_viewer;
        create_node_widget m_create_node_widget;

        std::vector<resource_info> m_script_infos;
        std::string m_opened_file;
        gtl::uuid m_selected_script;
        ax::NodeEditor::EditorContext* m_context;
        gcore::script_descriptor m_descriptor;
        std::string m_new_script_name;
        std::size_t m_create_script_type_index = 0;
        bool m_display = false;
    };
}