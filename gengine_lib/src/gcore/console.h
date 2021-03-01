#pragma once

#include <functional>
#include <unordered_map>
#include <any>
#include <vector>
#include <variant>

#define GCORE_ENABLE_CONSOLE() 1

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct console
    {
        using value = std::variant<int*, float*, bool*, std::string*, std::function<bool(std::string_view)>>;
        static console& get();

        template<class Callable>
        void register_callback(std::string_view name, Callable&& callable)
        {
            m_values[name] = std::forward<Callable>(callable);
            m_has_been_modified = true;
        }
        void register_value(std::string_view name, value value);
        void unregister_value(std::string_view name);
        bool execute(std::string_view console_view);

        std::vector<std::string_view> get_possible_commands() const;
        value const* get_value(std::string_view name) const;

        bool consume_has_been_modified();

    private:
        std::unordered_map<std::string_view, value> m_values;
        bool m_has_been_modified = false;
    };

#if GCORE_ENABLE_CONSOLE()
#define GCORE_REGISTER_CONSOLE_VALUE(DisplayName, Type, VariableName, DefaultValue)                                 \
    Type VariableName = (gcore::console::get().register_value(DisplayName, &VariableName), DefaultValue );
#else
#define GCORE_REGISTER_CONSOLE_VALUE(DisplayName, Type, VariableName, DefaultValue)                                 \
        Type const VariableName = DefaultValue;
#endif //GCORE_ENABLE_CONSOLE
}