#pragma once

#include "gtool/window.h"

#include "ax_NodeEditor/imgui_node_editor.h"

#include "gcore/script/script.h"

namespace gtool
{
    struct node_viewer_widget
    {
        void update(gcore::script_descriptor& script_descriptor);
        gcore::node_id_t m_node_id = 0;
    };

    struct node_editor_window : window
    {
        node_editor_window();
        ~node_editor_window();

        void update(gcore::world& world, window_manager& manager) override;
        const char* get_name() const override { return "Node Editor"; }
        virtual bool& get_should_display() { return m_display; }

    private:
        void open_file();
        void save_file();
        void display_node(gcore::script_descriptor::node_descriptor& node_desc);
        void create_new_elements();
        void delete_elements();
        void create_new_nodes();
        void setup_new_node(std::unique_ptr<gcore::node> new_node);

        node_viewer_widget m_node_viewer;
        ImGuiTextFilter m_node_filter;
        std::string m_opened_file;
        std::vector<std::string> m_possible_type_names;
        ax::NodeEditor::EditorContext* m_context;
        gcore::script_descriptor m_descriptor;
        bool m_display = false;
    };
}