#include "stdafx.h"
#include "grender/serializers/imgui_serializer.h"

#include <imgui.h>
#include "imgui_impl/imgui_stdlib.h"

#include "gtl/uuid.h"

namespace grender
{
    imgui_serializer::imgui_serializer(const char* node_name)
    {
        m_should_display_stack.push_back(ImGui::TreeNode(node_name));
    }

    imgui_serializer::~imgui_serializer()
    {
        if (should_display())
        {
            ImGui::TreePop();
        }
    }

    void imgui_serializer::process(const char* name, std::string& value)
    {
        if(should_display())
            ImGui::InputText(name, &value);
    }

    void imgui_serializer::process(const char* name, std::uint32_t& value)
    {
        if(should_display())
            ImGui::InputScalar(name, ImGuiDataType_U32, &value);
    }

    void imgui_serializer::process(const char* name, std::uint64_t& value)
    {
        if(should_display())
            ImGui::InputScalar(name, ImGuiDataType_U64, &value);
    }

    void imgui_serializer::process(const char* name, std::int32_t& value)
    {
        if (should_display())
            ImGui::InputInt(name, &value);
    }

    void imgui_serializer::process(const char* name, std::int64_t& value)
    {
        if(should_display())
            ImGui::InputScalar(name, ImGuiDataType_S64, &value);
    }

    void imgui_serializer::process(const char* name, float& value)
    {
        if (should_display())
            ImGui::InputFloat(name, &value);
    }

    void imgui_serializer::process(const char* name, double& value)
    {
        if (should_display())
            ImGui::InputDouble(name, &value);
    }

    void imgui_serializer::process(const char* name, bool& value)
    {
        if (should_display())
            ImGui::Checkbox(name, &value);
    }

    void imgui_serializer::process(const char* name, gtl::uuid& value)
    {
        if (should_display())
        {
            std::string string = value.to_string();
            ImGui::InputText(name, &string);
            value = gtl::uuid::from_string(string);
        }
    }

    void imgui_serializer::open_scope(const char* name)
    {
        if (should_display())
        {
            m_should_display_stack.push_back(ImGui::TreeNode(name));
        }
        else
        {
            m_should_display_stack.push_back(false);
        }
    }

    void imgui_serializer::close_scope(const char*)
    {
        if (should_display())
        {
            ImGui::TreePop();
        }
        m_should_display_stack.pop_back();
    }

    void imgui_serializer::open_array(const char* name, std::size_t& element_count)
    {
        if (should_display())
        {
            m_should_display_stack.push_back(ImGui::TreeNode(name));
        }
        else
        {
            m_should_display_stack.push_back(false);
        }
        m_array_index_stack.push_back(0);
        m_array_element_stack.push_back(element_count);
    }

    void imgui_serializer::close_array(const char* name)
    {
        if (should_display())
        {
            ImGui::TreePop();
        }
        m_array_index_stack.pop_back();
        m_should_display_stack.pop_back();
        m_array_element_stack.pop_back();
    }

    bool imgui_serializer::open_array_element(const char* element_name)
    {
        std::size_t const value = m_array_index_stack.back()++;
        if ((value < m_array_element_stack.back()) || (should_display() && ImGui::Button("Create Element")))
        {
            if (should_display())
            {
                m_should_display_stack.push_back(ImGui::TreeNode(reinterpret_cast<void const*>(value), element_name));
            }
            else
            {
                m_should_display_stack.push_back(false);
            }
            return true;
        }

        return false;
    }

    void imgui_serializer::close_array_element(const char* element_name)
    {
        close_scope(element_name);
    }
}
