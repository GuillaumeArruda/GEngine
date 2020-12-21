#include "stdafx.h"
#include "resource_handle.h"

#include "gcore/resource.h"
#include "gcore/resource_library.h"

#include "gserializer/serializer.h"

namespace gcore
{
    resource_proxy::resource_proxy(resource* target, resource_library* res_lib)
        : m_target(target)
        , m_reslib(res_lib)
    {
    }

    resource_proxy::~resource_proxy()
    {
        unload_resource();
    }

    resource_proxy& resource_proxy::operator=(resource_proxy&& move) noexcept
    {
        if (this == &move)
            return *this;

        unload_resource();
        m_reslib = std::move(move.m_reslib);
        m_target = move.m_target;
        move.m_target = nullptr;

        return (*this);
    }

    bool resource_proxy::is_loaded() const { return m_target && m_target->is_loaded(); }

    void resource_proxy::set_target(resource* target)
    {
        m_target = target;
    }

    void resource_proxy::reset()
    {
        m_target = nullptr;
        m_reslib = nullptr;
    }

    void resource_proxy::unload_resource()
    {
        if (m_target && m_reslib)
        {
            m_reslib->request_unload(m_target);
        }
    }

    void process(gserializer::serializer& serializer, gtl::uuid& uuid, gcore::resource_handle<gcore::resource>& target_handle)
    {
        using gserializer::process;
        process(serializer, uuid);
        if (serializer.is_writing_to_object())
        {
            if (auto* res_lib = serializer.get_in_context<std::reference_wrapper<gcore::resource_library>>())
            {
                target_handle = res_lib->get().get_resource<gcore::resource>(uuid);
            }
        }
    }
}
