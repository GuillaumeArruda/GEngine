#pragma once

#include <memory>

#include "gtl/uuid.h"
#include "gtl/cast.h"

namespace gserializer
{
    struct serializer;
}


namespace gcore
{
    struct resource;
    struct resource_library;

    struct resource_proxy
    {
        resource_proxy() = default;
        resource_proxy(resource* target, resource_library* res_lib);
        ~resource_proxy();

        resource_proxy(resource_proxy&& move) noexcept;
        resource_proxy& operator=(resource_proxy&& move) noexcept;

        resource_proxy(resource_proxy const&) = delete;
        resource_proxy& operator=(resource_proxy const&) = delete;

        explicit operator bool() const noexcept
        {
            return m_target;
        }

        bool is_loaded() const;

        void set_target(resource* target);
        resource* get_target() { return m_target; }
        resource const* get_target() const { return m_target; }

    private:
        friend resource_library;
        void reset();
        void unload_resource();
        resource* m_target = nullptr;
        resource_library* m_reslib = nullptr;
    };

    template<class ResourceType>
    struct resource_handle
    {
        resource_handle(std::shared_ptr<resource_proxy> proxy = nullptr) : m_proxy(proxy) {}
        
        template<class U>
        resource_handle(resource_handle<U> const& copy) : m_proxy(copy.m_proxy) { verify_type(); }
        template<class U>
        resource_handle(resource_handle<U>&& move) : m_proxy(std::move(move.m_proxy)) { verify_type(); }

        template<class U>
        resource_handle& operator=(resource_handle<U>&& move) 
        {
            m_proxy = std::move(move.m_proxy);
            verify_type();
            return *this;
        }

        template<class U>
        resource_handle& operator=(resource_handle<U> const& copy)
        {
            m_proxy = copy.m_proxy;
            verify_type();
            return *this;
        }

        explicit operator resource_handle<resource>() const noexcept
        {
            return resource_handle<resource>(m_proxy);
        }

        ResourceType* get() { return m_proxy ? dynamic_cast<ResourceType*>(m_proxy->get_target()) : nullptr; }
        ResourceType const* get() const { return m_proxy ? dynamic_cast<ResourceType const*>(m_proxy->get_target()) : nullptr; }

        ResourceType* operator->() { return get(); }
        ResourceType const* operator->() const { return get(); }

        ResourceType& operator*() { return *get(); }
        ResourceType const& operator*() const { return *get(); }

        explicit operator bool() const noexcept
        {
            return m_proxy && m_proxy->get_target();
        }

        template<class ResType>
        ResType* as()
        {
            return dynamic_cast<ResType*>(get());
        }

        template<class ResType>
        ResType const* as() const
        {
            return dynamic_cast<ResType const*>(get());
        }

        bool is_loaded() const { return m_proxy && m_proxy->is_loaded(); }

        void reset() { m_proxy.reset(); }

    private:
        void verify_type()
        {
            if (m_proxy)
            {
                gtl::cast<ResourceType*>(m_proxy->get_target());
            }
        }
        template<class U>
        friend struct resource_handle;
        std::shared_ptr<resource_proxy> m_proxy;
    };

    void process(gserializer::serializer& serializer, gtl::uuid& uuid, gcore::resource_handle<gcore::resource>& target_handle);

    template<class ResourceType>
    void process(gserializer::serializer& serializer, gtl::uuid& uuid, gcore::resource_handle<ResourceType>& target_handle)
    {
        gcore::resource_handle<gcore::resource> temp(target_handle);
        process(serializer, uuid, temp);
        if (static_cast<bool>(temp) == uuid.is_valid() && temp.as<ResourceType>())
        {
            if (temp && !temp.as<ResourceType>())
            {
                uuid = gtl::uuid();
            }
            else
            {
                target_handle = std::move(temp);
            }
        }
    }

    template<class ResourceType>
    void process(gserializer::serializer&, gcore::resource_handle<ResourceType>&)
    {
    }
}

