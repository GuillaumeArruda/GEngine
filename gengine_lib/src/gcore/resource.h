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
            initial,
            pending_other_resources,
            pending_async,
            pending_sync,
            loaded,
            failed,
            unloaded
        };
        resource();

        resource(resource const&) = delete;
        resource(resource&&) = delete;
        virtual ~resource();

        resource& operator=(resource const&) = delete;
        resource& operator=(resource&&) = delete;

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

        virtual void create_loading_dependent_resources();
        gtl::span<gcore::resource_handle<resource> const> get_loading_dependent_resources() const;

    protected:
        resource_library* m_library = nullptr;
        std::vector<gcore::resource_handle<resource>> m_dependent_resources;

    private:
        virtual bool do_load_async() { return true; };
        virtual bool do_load_sync() = 0;
        virtual void do_unload() = 0;

        std::string m_name;
        gtl::uuid m_uuid = gtl::uuid::generate();
        loading_state m_loading_state = loading_state::initial;
    };
}