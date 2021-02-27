#pragma once

#include <functional>
#include <unordered_map>
#include <any>
#include <vector>
#include <variant>

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct console
    {
        using value = std::variant<int*, float*, bool*, std::string*, std::function<bool(std::string_view arguments)>>;
        static console& get();

        template<class Callable>
        void register_callback(std::string_view name, Callable&& callable)
        {
            m_values[name] = std::forward<Callable>(callable);
        }
        void register_value(std::string_view name, value value);
        bool execute(std::string_view console_view);

        std::vector<std::string_view> get_possible_commands() const;

    private:
        std::unordered_map<std::string_view, value> m_values;
    };

#define GCORE_REGISTER_CONSOLE_VALUE(DisplayName, Type, VariableName, DefaultValue)                                 \
    Type VariableName = (gcore::console::get().register_value(DisplayName, &VariableName), DefaultValue );

}