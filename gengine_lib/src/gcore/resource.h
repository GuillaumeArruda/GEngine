#pragma once

#include "gtl/uuid.h"
#include "gserializer/type_factory.h"


namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct resource_library;
    template<class ResType>
    struct resource_handle;
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
        virtual ~resource();

        virtual void process(gserializer::serializer& serializer);

        void load_async();
        void load_sync();
        void unload();

        gtl::uuid const& get_uuid() const noexcept{ return m_uuid; }
        std::string const& get_name() const noexcept { return m_name; }
        bool is_loaded() const { return m_loading_state == loading_state::loaded; }

        loading_state get_state() const { return m_loading_state; }

        GSERIALIZER_DECLARE_FACTORY_BASE(resource);

        void set_name(std::string name) { m_name = std::move(name); }
        void set_library(resource_library* library) { m_library = library; }

    protected:
        void ensure_all_dependant_resources_loaded(std::vector<resource_handle<resource>> const& resources);

    private:
        virtual bool do_load_async() { return true; };
        virtual bool do_load_sync() = 0;
        virtual void do_unload() = 0;

        std::string m_name;
        gtl::uuid m_uuid = gtl::uuid::generate();
        loading_state m_loading_state = loading_state::pending_async;
        resource_library* m_library = nullptr;
    };
}