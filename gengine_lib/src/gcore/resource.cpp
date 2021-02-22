#include "stdafx.h"

#include "gcore/resource.h"
#include "gcore/resource_handle.h"
#include "gcore/resource_library.h"
#include "gcore/serializers/dependency_gatherer_serializer.h"

#include "gserializer/serializer.h"

#include "gtl/scope_timer.h"


namespace gcore
{
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
        dependency_gatherer_serializer gatherer(true);
        gatherer.set_in_context(std::ref(*m_library));
        process(gatherer);
        std::vector<resource_handle<resource>> resources;
        for (gtl::uuid const& uuid : gatherer.m_uuids)
        {
            if (m_uuid == uuid)
                continue;

            if (resource_handle<resource> res = m_library->try_get_resource<resource>(uuid))
            {
                resources.push_back(std::move(res));
            }
        }
        ensure_all_dependant_resources_loaded(resources);

        assert(m_loading_state == loading_state::pending_async);
        std::string name = "load_async " + m_name;
        gtl::scope_timer const s(name.c_str());
        if (do_load_async())
        {
            m_loading_state = loading_state::pending_sync;
        }
        else
        {
            m_loading_state = loading_state::failed;
        }
    }

    void resource::load_sync()
    {
        assert(m_loading_state == loading_state::pending_sync || m_loading_state == loading_state::failed);
        std::string name = "load_sync " + m_name;
        gtl::scope_timer const s(name.c_str());
        if (m_loading_state == loading_state::pending_sync)
        {
            if (do_load_sync())
            {
                m_loading_state = loading_state::loaded;
            }
            else
            {
                m_loading_state = loading_state::failed;
            }
        }
    }

    void resource::unload()
    {
        assert(m_loading_state == loading_state::loaded);
        std::string name = "unload " + m_name;
        gtl::scope_timer const s(name.c_str());
        do_unload();
        m_loading_state = loading_state::unloaded;
    }

    void resource::ensure_all_dependant_resources_loaded(std::vector<resource_handle<resource>> const& resources)
    {
        while (resources.size() > 0 && std::find_if(resources.begin(), resources.end(), [](resource_handle<resource> const& res) { return !res.is_loaded(); }) != resources.end())
        {
            m_library->run_loading_job();
        }
    }
}

