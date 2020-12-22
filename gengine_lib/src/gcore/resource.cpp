#include "stdafx.h"

#include "gcore/resource.h"

#include "gserializer/serializer.h"

namespace gcore
{
    void resource::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid", m_uuid);
        serializer.process("name", m_name);
    }

    void resource::load_async()
    {
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
        if (m_loading_state == loading_state::loaded)
        {
            do_unload();
            m_loading_state = loading_state::unloaded;
        }
    }
}

