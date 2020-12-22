#include "stdafx.h"

#include "gcore/resource.h"

#include "gserializer/serializer.h"

#include "gtl/scope_timer.h"

namespace gcore
{
    resource::~resource() 
    { 
        assert(m_loading_state == loading_state::failed || m_loading_state == loading_state::unloaded); 
    }
    
    void resource::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid", m_uuid);
        serializer.process("name", m_name);
    }

    void resource::load_async()
    { 
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
}

