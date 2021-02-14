#include "stdafx.h"

#include <imgui/imgui.h>
#include "imgui/imgui_stdlib.h"

#include "gtool/windows/node_editor_window.h"

#include "gserializer/serializers/json_serializer.h"

#include "gcore/script/script_context.h"
#include "gcore/script/nodes/constant_node.h"
#include "gcore/script/node_factory.h"

#include "grender/serializers/imgui_serializer.h"

namespace gtool
{
    struct node_pin_id
    {
        node_pin_id(std::uint64_t id = 0) : m_id(id) {}
        node_pin_id(gcore::node_id_t node_id, int pin_id) { set(node_id, pin_id); }
        
        void set(gcore::node_id_t node_id, int pin_id) { m_id = static_cast<uint64_t>(node_id) << 32 | pin_id; }
        gcore::node_id_t get_node_id() const { return static_cast<gcore::node_id_t>(m_id >> 32); }
        int get_pin_id() const { return static_cast<int>(m_id); }
        
        std::uint64_t m_id;
    };

    node_editor_window::node_editor_window()
        : m_possible_type_names(gcore::node_factory::get().get_possible_type_names())
    {
        m_context = ax::NodeEditor::CreateEditor();
    }

    node_editor_window::~node_editor_window()
    {
        ax::NodeEditor::DestroyEditor(m_context);
    }

    void node_editor_window::open_file()
    {
        ax::NodeEditor::DestroyEditor(m_context);
        m_context = ax::NodeEditor::CreateEditor();

        ax::NodeEditor::SetCurrentEditor(m_context);
        gserializer::json_read_serializer serializer(m_opened_file.c_str());
        m_descriptor = gcore::script_descriptor();
        m_descriptor.process(serializer);

        m_descriptor.m_node_id_generator = 0;
        for (gcore::script_descriptor::node_descriptor const& node_desc : m_descriptor.m_nodes)
        {
            m_descriptor.m_node_id_generator = std::max(m_descriptor.m_node_id_generator, node_desc.m_node->get_node_id());
            ax::NodeEditor::SetNodePosition(node_desc.m_node->get_node_id(), ImVec2(node_desc.m_position.x, node_desc.m_position.y));
        }
        m_descriptor.m_node_id_generator++;
        ax::NodeEditor::SetCurrentEditor(nullptr);
    }

    void node_editor_window::save_file()
    {
        ax::NodeEditor::SetCurrentEditor(m_context);
        for (gcore::script_descriptor::node_descriptor& node_desc : m_descriptor.m_nodes)
        {
            ImVec2 const pos = ax::NodeEditor::GetNodePosition(node_desc.m_node->get_node_id());
            node_desc.m_position.x = pos.x;
            node_desc.m_position.y = pos.y;
        }
        ax::NodeEditor::SetCurrentEditor(nullptr);

        gserializer::json_write_serializer serializer;
        m_descriptor.process(serializer);
        serializer.write_to_file(m_opened_file.c_str());
    }

    void node_editor_window::update(gcore::world&, window_manager&)
    {
        namespace ne = ax::NodeEditor;
        if (m_display)
        {
            ImGuiWindowFlags const window_flags = ImGuiWindowFlags_MenuBar;
            if (!ImGui::Begin(get_name(), &m_display, window_flags))
            {
                ImGui::End();
                return;
            }

            const char* menu_action = nullptr;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open File"))
                    {
                        menu_action = "Open File Pop Up";
                    }
                    if (ImGui::MenuItem("Save File"))
                    {
                        menu_action = "Save File Pop Up";
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (menu_action)
            {
                ImGui::OpenPopup(menu_action);
            }

            if (ImGui::BeginPopup("Open File Pop Up"))
            {
                ImGui::InputText("File", &m_opened_file);
                if (ImGui::Button("Open"))
                {
                    open_file();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopup("Save File Pop Up"))
            {
                ImGui::InputText("File", &m_opened_file);
                if (ImGui::Button("Save"))
                {
                    save_file();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }


            ne::SetCurrentEditor(m_context);
            
            ne::Begin(get_name());
            {
                for (gcore::script_descriptor::node_descriptor& node_desc : m_descriptor.m_nodes)
                {
                    display_node(node_desc);
                }

                for (std::pair<gcore::node_id_t, std::vector<gcore::script_descriptor::connection>> const& connections : m_descriptor.m_connections)
                {
                    for (gcore::script_descriptor::connection const& connection : connections.second)
                    {
                        node_pin_id const destination_id(connections.first, connection.m_destination_pin_id);
                        node_pin_id const source_id(connection.m_source_id, connection.m_source_pin_id);
                        ne::Link(destination_id.m_id, source_id.m_id, destination_id.m_id);
                    }
                }
            }

            create_new_nodes();
            create_new_elements();
            delete_elements();

            std::vector<ne::NodeId> selected_nodes;
            selected_nodes.resize(ne::GetSelectedObjectCount());
            int const number_of_selected_nodes = ne::GetSelectedNodes(selected_nodes.data(), static_cast<int>(selected_nodes.size()));
            selected_nodes.resize(number_of_selected_nodes);
            if (selected_nodes.size() == 1)
            {
                m_node_viewer.m_node_id = static_cast<gcore::node_id_t>(selected_nodes.back().Get());
            }
            else
            {
                m_node_viewer.m_node_id = 0;
            }

            ne::End();
            ne::SetCurrentEditor(nullptr);

            m_node_viewer.update(m_descriptor);

            ImGui::End();
        }
    }

    void node_editor_window::display_node(gcore::script_descriptor::node_descriptor& node_desc)
    {
        namespace ne = ax::NodeEditor;
        gcore::node_id_t const node_id = node_desc.m_node->get_node_id();
        ne::BeginNode(node_id);
        if (!node_desc.m_name.empty())
        {
            ImGui::TextUnformatted(node_desc.m_name.c_str());
        }
        ImGui::TextUnformatted(std::string(gcore::node_factory::get().get_display_name(*node_desc.m_node)).c_str());

        gcore::node::pin_descriptors pin_descriptors = node_desc.m_node->get_pin_descriptors();

        ImGui::BeginGroup();

        for (gcore::pin_descriptor const& input_pin : pin_descriptors[gtl::to_underlying(gcore::pin_type::input)])
        {
            node_pin_id const node_pin_id(node_id, input_pin.m_id);
            ne::BeginPin(node_pin_id.m_id, ne::PinKind::Input);
                ImGui::Text("->"); ImGui::SameLine();
                ImVec2 const min = ImGui::GetItemRectMin();
                ImVec2 const max = ImGui::GetItemRectMax();
                ne::PinRect(min, max);
                ImGui::Text(input_pin.m_name);
                
            ne::EndPin();
        }
        ImGui::EndGroup();
        ImGui::SameLine();

        ImGui::BeginGroup();
        for (gcore::pin_descriptor const& output_pin : pin_descriptors[gtl::to_underlying(gcore::pin_type::output)])
        {
            node_pin_id const node_pin_id(node_id, output_pin.m_id);
            ne::BeginPin(node_pin_id.m_id, ne::PinKind::Output);
                ImGui::Text(output_pin.m_name); ImGui::SameLine();
                ImGui::Text("->");
                ImVec2 const min = ImGui::GetItemRectMin();
                ImVec2 const max = ImGui::GetItemRectMax();
                ne::PinRect(min, max);
            ne::EndPin();
        }
        ImGui::EndGroup();
        ne::EndNode();
    }

    void node_editor_window::create_new_elements()
    {
        namespace ne = ax::NodeEditor;
        if (ne::BeginCreate())
        {
            ne::PinId in_id;
            ne::PinId out_id;
            if (ne::QueryNewLink(&in_id, &out_id))
            {
                if (in_id && out_id)
                {
                    node_pin_id source_node_pin_id(in_id.Get());
                    node_pin_id const destination_node_pin_id(out_id.Get());

                    if (m_descriptor.can_add_connection(source_node_pin_id.get_node_id(), source_node_pin_id.get_pin_id(), destination_node_pin_id.get_node_id(), destination_node_pin_id.get_pin_id()))
                    {
                        if (ne::AcceptNewItem())
                        {
                            gcore::node* conversion_node = nullptr;
                            if (m_descriptor.add_connection(source_node_pin_id.get_node_id(), source_node_pin_id.get_pin_id(), destination_node_pin_id.get_node_id(), destination_node_pin_id.get_pin_id(), conversion_node))
                            {
                                if (conversion_node)
                                {
                                       ImVec2 const from_position = ne::GetNodePosition(source_node_pin_id.get_node_id());
                                       ImVec2 const to_position = ne::GetNodePosition(destination_node_pin_id.get_node_id());
                                       ImVec2 conversion_position;
                                       conversion_position.x = (from_position.x + to_position.x) / 2.f;
                                       conversion_position.y = (from_position.y + to_position.y) / 2.f;
                                       ne::SetNodePosition(conversion_node->get_node_id(), conversion_position);
                                }
                            }
                        }
                    }
                    else
                    {
                        ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    }
                }
            }
        }
        ne::EndCreate();
    }

    void gtool::node_editor_window::delete_elements()
    {
        namespace ne = ax::NodeEditor;
        if (ne::BeginDelete())
        {
            ne::LinkId link_id;
            while (ne::QueryDeletedLink(&link_id))
            {
                if (ne::AcceptDeletedItem())
                {
                    node_pin_id const destination_node_pin_id(link_id.Get());
                    m_descriptor.remove_connection(destination_node_pin_id.get_node_id(), destination_node_pin_id.get_pin_id());
                }
            }

            ne::NodeId node;
            while (ne::QueryDeletedNode(&node))
            {
                if (node && ne::AcceptDeletedItem())
                {
                    gcore::node_id_t const node_id = static_cast<gcore::node_id_t>(node.Get());
                    auto const connection_it = m_descriptor.m_connections.find(static_cast<gcore::node_id_t>(node_id));
                    if (connection_it != m_descriptor.m_connections.end())
                        m_descriptor.m_connections.erase(connection_it);

                    auto const node_it = std::find_if(m_descriptor.m_nodes.begin(), m_descriptor.m_nodes.end(), [&](gcore::script_descriptor::node_descriptor const& node_desc) { return node_desc.m_node->get_node_id() == node_id; });
                    if (node_it != m_descriptor.m_nodes.end())
                        m_descriptor.m_nodes.erase(node_it);
                }
            }
        }
        ne::EndDelete();
    }

    void node_editor_window::create_new_nodes()
    {
        namespace ne = ax::NodeEditor;
        if (ne::ShowBackgroundContextMenu())
        {
            ne::Suspend();
            ImGui::OpenPopup("Create Node");
            ne::Resume();
        }

        ne::Suspend();
        if (ImGui::BeginPopup("Create Node"))
        {
            std::unique_ptr<gcore::node> new_node;
            gcore::node_factory& factory = gcore::node_factory::get();

            m_node_filter.Draw();
            for (std::string const& type_name : m_possible_type_names)
            {
                if (m_node_filter.PassFilter(type_name.c_str()))
                {
                    if (ImGui::MenuItem(type_name.c_str()))
                    {
                        new_node = factory.create(type_name);
                    }
                }
            }

            if (new_node)
            {
                setup_new_node(std::move(new_node));
            }

            ImGui::EndPopup(); 
        }

        if (!ImGui::IsPopupOpen("Create Node"))
        {
            m_node_filter.Clear();
        }

        ne::Resume();
    }

    void node_editor_window::setup_new_node(std::unique_ptr<gcore::node> new_node)
    {
        namespace ne = ax::NodeEditor;
        gcore::script_descriptor::node_descriptor node_descriptor;
        node_descriptor.m_node = std::move(new_node);
        node_descriptor.m_node->set_node_id(m_descriptor.m_node_id_generator++);
        gcore::node_id_t const new_node_id = node_descriptor.m_node->get_node_id();
        m_descriptor.m_nodes.push_back(std::move(node_descriptor));

        gcore::node::pin_descriptors const pin_descriptors = m_descriptor.m_nodes.back().m_node->get_pin_descriptors();

        auto const node_position = ne::ScreenToCanvas(ImGui::GetMousePos());
        constexpr int constant_node_x_offset = -80;
        constexpr int constant_node_y_offset = 50;
        auto constant_position = node_position;
        constant_position.x += constant_node_x_offset;

        for (gcore::pin_descriptor const& input : pin_descriptors[gtl::to_underlying(gcore::pin_type::input)])
        {
            if (input.m_is_optional)
                continue;
           
            gcore::script_descriptor::node_descriptor constant_descriptor;
            constant_descriptor.m_node = std::make_unique<gcore::constant_node>();
            constant_descriptor.m_node->set_node_id(m_descriptor.m_node_id_generator++);
            gcore::node_id_t const node_id = constant_descriptor.m_node->get_node_id();
            static_cast<gcore::constant_node*>(constant_descriptor.m_node.get())->get_node_data().set_type_id(input.m_type_id);
            m_descriptor.m_nodes.push_back(std::move(constant_descriptor));

            gcore::node* node = nullptr;
            m_descriptor.add_connection(node_id, gcore::constant_node::output_id, new_node_id, input.m_id, node);
            ne::SetNodePosition(node_id, constant_position);
            constant_position.y += constant_node_y_offset;
        }

        ne::SetNodePosition(new_node_id, node_position);
    }

    void node_viewer_widget::update(gcore::script_descriptor& script_descriptor)
    {
        if (ImGui::Begin("Node viewer"))
        {
            auto const it = std::find_if(script_descriptor.m_nodes.begin(), script_descriptor.m_nodes.end(), [&](gcore::script_descriptor::node_descriptor const& node_desc) { return node_desc.m_node->get_node_id() == m_node_id; });
            if (it != script_descriptor.m_nodes.end())
            {
                ImGui::PushID(m_node_id);
                grender::imgui_serializer serializer("Node Data");
                it->process(serializer);
                ImGui::PopID();
            }
        }
        ImGui::End();
    }
}