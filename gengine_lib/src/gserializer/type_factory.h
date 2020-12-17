#pragma once

#include <unordered_map>
#include <string_view>
#include <functional>
#include <typeinfo>
#include <typeindex>
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
            m_type_id_hash_to_name[std::type_index(typeid(T))] = name;
            m_name_to_type_id.insert(std::pair(name,std::type_index(typeid(T))));
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
            auto it = m_type_id_hash_to_name.find(std::type_index(typeid(value)));
            if (it != m_type_id_hash_to_name.end())
            {
                return it->second;
            }
            return {};
        }

        std::type_index get_type_index_from_name(std::string_view type_name)
        {
            auto it = m_name_to_type_id.find(type_name);
            if (it != m_name_to_type_id.end())
            {
                return it->second;
            }
            return std::type_index(typeid(void));
        }

        std::vector<std::string> get_possible_type_names() const
        {
            std::vector<std::string> rv;
            rv.reserve(m_type_id_hash_to_name.size());
            for (auto& pair : m_type_id_hash_to_name)
            {
                rv.push_back(pair.second);
            }
            return rv;
        }

    private:
        std::unordered_map<std::string_view, std::function<std::unique_ptr<BaseClass>()>> m_creation_map;
        std::unordered_map<std::type_index, std::string> m_type_id_hash_to_name;
        std::unordered_map<std::string_view, std::type_index> m_name_to_type_id;
    };

#define GSERIALIZER_DECLARE_FACTORY_BASE(BaseType)                                                                                                      \
    static gserializer::type_factory<BaseType>& factory() { static gserializer::type_factory<BaseType> factory_##BaseType; return factory_##BaseType; } 

#define GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION()         \
    static bool ms_registered;                                      

#define GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(Type)      \
    bool Type::ms_registered = Type::factory().register_type<Type>(#Type);
}
