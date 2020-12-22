#pragma once

#include "gtl/uuid.h"
#include "gserializer/type_factory.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct resource
    {
        enum class loading_state
        {
            pending_async,
            pending_sync,
            loaded,
            failed,
            unloaded
        };
        virtual ~resource() {};

        virtual void process(gserializer::serializer& serializer);

        void load_async();
        void load_sync();
        void unload();

        gtl::uuid const& get_uuid() const noexcept{ return m_uuid; }
        bool is_loaded() const { return m_loading_state == loading_state::loaded; }

        loading_state get_state() const { return m_loading_state; }

        GSERIALIZER_DECLARE_FACTORY_BASE(resource);

    private:
        virtual bool do_load_async() { return true; };
        virtual bool do_load_sync() = 0;
        virtual void do_unload() = 0;

        loading_state m_loading_state = loading_state::pending_async;
        gtl::uuid m_uuid = gtl::uuid::generate();
        std::string m_name;
    };
}