#pragma once

#include <vector>
#include <string>

namespace gtool
{
    namespace details
    {
        struct factory_combo_box_base
        {
            void draw(const char* name);
            const char* get_current_type() const { return m_current_type; }
        protected:
            std::vector<std::string> m_type_names;
            const char* m_current_type = nullptr;
        };
    }

    template<class FactoryOwnerClass>
    struct factory_combo_box : details::factory_combo_box_base
    {
        factory_combo_box()
        {
            m_type_names = FactoryOwnerClass::factory().get_possible_type_names();
            if (m_type_names.size() > 0)
            {
                m_current_type = m_type_names.front().c_str();
            }
        }
    };
}