#include "stdafx.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include "gtool/windows/console_window.h"

#include "gcore/console.h"

namespace gtool
{
    console_window::console_window()
    {
        gcore::console::get().register_callback("console.clear", [&](std::string_view) { clear_log(); return true; });
        gcore::console::get().register_callback("console.clear_history", [&](std::string_view) { clear_history(); return true; });
        m_possible_commands = gcore::console::get().get_possible_commands();
        m_suggestions.reserve(3);
    }

    void console_window::update(gcore::world&, window_manager&)
    {
        if (!m_should_display)
            return;
        if (!ImGui::Begin(get_name(), &m_should_display))
        {
            ImGui::End();
            return;
        }

        const float footer_height_to_reserve = (ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing()) * max_number_of_suggestion;
        ImGui::BeginChild("scrolling_region", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
            for (log_entry const& entry : m_log)
            {
                ImVec4 const color = entry.m_success ? ImVec4(0.4f, 1.f, 0.4f, 1.f) : ImVec4(1.f, 0.4f, 0.4f, 1.f);
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(entry.m_command.c_str());
                ImGui::PopStyleColor();
            }
            ImGui::PopStyleVar();
        ImGui::EndChild();

        ImGui::Separator();

        ImGuiInputTextFlags const flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackEdit;
        if (ImGui::InputText("Input", &m_current_input, flags, [](ImGuiInputTextCallbackData* data) { return static_cast<console_window*>(data->UserData)->text_edit_callback(data); }, static_cast<void*>(this)))
        {
            bool const success = gcore::console::get().execute(m_current_input);
            m_history.push_back(m_current_input);
            m_log.push_back(log_entry{ std::move(m_current_input), success });
            m_current_input.clear();
            m_suggestions.clear();
            m_selected_history_index.reset();
            ImGui::SetKeyboardFocusHere();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
        for (std::string const& suggestion : m_suggestions)
        {
            ImGui::TextUnformatted(suggestion.c_str());
        }
        ImGui::PopStyleVar();
        
        ImGui::End();
    }

    int console_window::text_edit_callback(ImGuiInputTextCallbackData* data)
    {
        switch (data->EventFlag)
        {
        case ImGuiInputTextFlags_CallbackEdit:
        {
            m_suggestions.clear();
            if (data->BufSize != 0)
            {
                for (std::string_view const& possible_command : m_possible_commands)
                {
                    if (m_suggestions.size() < max_number_of_suggestion)
                    {
                        if (possible_command.find(data->Buf) != std::string_view::npos)
                        {
                            m_suggestions.push_back(std::string(possible_command));
                        }
                    }
                }
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackCompletion: 
        {
            if (m_suggestions.size() != 0)
            {
                data->DeleteChars(0, static_cast<int>(m_current_input.size()));
                data->InsertChars(0, m_suggestions.front().c_str());
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            if (m_history.size() == 0)
                return 0;

            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (m_selected_history_index && m_selected_history_index.value() != 0)
                {
                    m_selected_history_index.value()--;
                }
                else
                {
                    m_selected_history_index = m_history.size() - 1;
                }

            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (m_selected_history_index && m_selected_history_index.value() != (m_history.size() - 1))
                {
                    m_selected_history_index.value()++;
                }
                else
                {
                    m_selected_history_index = 0;
                }
            }

            if (m_selected_history_index)
            {
                data->DeleteChars(0, static_cast<int>(m_current_input.size()));
                data->InsertChars(0, m_history[m_selected_history_index.value()].c_str());
            }
            break;
        }
        }
        return 0;
    }

    void console_window::clear_history()
    {
        m_history.clear();
        m_selected_history_index.reset();
    }
    void console_window::clear_log()
    {
        m_log.clear();
    }
}
