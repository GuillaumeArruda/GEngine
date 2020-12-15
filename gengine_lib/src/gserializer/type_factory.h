#pragma once

#include <unordered_map>
#include <string_view>
#include <functional>
#include <typeinfo>
#include <string>

namespace gserializer
{
    template<class BaseClass>
    struct type_factory
    {
        template<class T>
        bool register_type(std::string_view name)
        {
            m_creation_map[name] = [] {return std::make_unique<T>(); };
            m_type_id_hash_to_name[typeid(T).hash_code()] = name;
            return true;
        }
        
        std::unique_ptr<BaseClass> create(std::string_view type_name) const
        {
            auto it = m_creation_map.find(type_name);
            if (it != m_creation_map.end())
            {
                return it->second();
            }
            return std::unique_ptr<BaseClass>();
        }
        
        std::string_view get_type_name(BaseClass& value) const
        {
            auto it = m_type_id_hash_to_name.find(typeid(value).hash_code());
            if (it != m_type_id_hash_to_name.end())
            {
                return it->second;
            }
            return {};
        }
    private:
        std::unordered_map<std::string_view, std::function<std::unique_ptr<BaseClass>()>> m_creation_map;
        std::unordered_map<std::size_t, std::string> m_type_id_hash_to_name;
    };

#define GSERIALIZER_DECLARE_FACTORY_BASE(BaseType)                                                                                                      \
    static gserializer::type_factory<BaseType>& factory() { static gserializer::type_factory<BaseType> factory_##BaseType; return factory_##BaseType; } 

#define GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION()         \
    static bool ms_registered;                                      

#define GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(Type)      \
    bool Type::ms_registered = Type::factory().register_type<Type>(#Type);
}
