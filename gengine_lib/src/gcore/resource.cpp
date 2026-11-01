#include "stdafx.h"

#include <optick/optick.h>
#include <iostream>

#include "gcore/resource.h"
#include "gcore/resource_handle.h"
#include "gcore/resource_library.h"
#include "gcore/serializers/dependency_gatherer_serializer.h"

#include "gserializer/serializer.h"

#include "gtl/scope_timer.h"


namespace gcore
{
    resource::resource()
    {
    }

    resource::~resource()
    { 
    }
    
    void resource::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid", m_uuid);
        serializer.process("name", m_name);
    }

    void resource::load_async()
    { 
        OPTICK_EVENT();
        OPTICK_TAG("Res Name", m_name.c_str());

        assert(m_loading_state == loading_state::pending_async);
        if (do_load_async())
        {
            m_loading_state = loading_state::pending_sync;
        }
        else
        {
            std::cerr << m_name << " failed to load async.";
            m_loading_state = loading_state::failed;
        }
    }

    void resource::load_sync()
    {
        OPTICK_EVENT();
        OPTICK_TAG("Res Name", m_name.c_str());
        assert(m_loading_state == loading_state::pending_sync || m_loading_state == loading_state::failed);
        if (m_loading_state == loading_state::pending_sync)
        {
            if (do_load_sync())
            {
                m_loading_state = loading_state::loaded;
            }
            else
            {
                std::cerr << m_name << " failed to load sync.";
                m_loading_state = loading_state::failed;
            }
        }
    }

    void resource::unload()
    {
        OPTICK_EVENT();
        OPTICK_TAG("Res Name", m_name.c_str());
        assert(m_loading_state == loading_state::loaded);
        do_unload();
        m_loading_state = loading_state::unloaded;
    }

    void resource::create_loading_dependent_resources()
    {
        OPTICK_EVENT();
        OPTICK_TAG("Res Name", m_name.c_str());
        assert(m_loading_state == loading_state::initial);
        dependency_gatherer_serializer gatherer(true);
        gatherer.set_in_context(std::ref(*m_library));
        process(gatherer);
        for (gtl::uuid const& uuid : gatherer.m_uuids)
        {
            if (get_uuid() == uuid)
                continue;

            if (resource_handle<resource> res = m_library->try_get_resource<resource>(uuid))
            {
                m_dependent_resources.push_back(std::move(res));
            }
        }
        m_library->get_dependency_tracker().add_dependencies(get_uuid(), gatherer.m_uuids, gatherer.m_files);
        m_loading_state = loading_state::pending_async;
    }

    gtl::span<gcore::resource_handle<resource>const> resource::get_loading_dependent_resources() const 
    { 
        return m_dependent_resources; 
    }
}

