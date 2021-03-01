#pragma once

#include <vector>
#include <string>
#include <optional>

#include "gtool/window.h"

namespace gtool
{
    struct console_window : window
    {
        console_window();
        void update(gcore::world& world, window_manager& manager) override;
        const char* get_name() const override { return "Console"; }
        bool& get_should_display() override { return m_should_display; }

    private:
        struct log_entry
        {
            std::string m_command;
            bool m_success;
        };
        static std::size_t const max_number_of_suggestion = 3;
        int text_edit_callback(struct ImGuiInputTextCallbackData* data);
        void clear_history();
        void clear_log();
        bool display_value(std::string_view name);


        std::vector<std::string> m_history;
        std::vector<std::string> m_suggestions;
        std::vector<std::string_view> m_possible_commands;
        std::vector<log_entry> m_log;
        std::string m_current_input;
        std::optional<std::size_t> m_selected_history_index;
        bool m_should_display = false;
    };

}