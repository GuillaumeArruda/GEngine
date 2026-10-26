#include "stdafx.h"
#include "grender/nodes/render_mesh_node.h"
#include "grender/systems/render_system.h"

namespace grender
{
    void render_mesh_node::execute(gcore::node_context& context) const
    {
        if (!m_mesh.is_loaded())
            return;
        
        gtl::span<bool const> enabled = context.read<in_enable_pin>();
        if (enabled.empty() || std::none_of(enabled.begin(), enabled.end(), [](bool const enable) { return enable; }))
            return;

        auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>();
        if (!render_sys)
            return;

        gtl::span<glm::mat4 const> transform = context.read<in_transform_pin>();
        if (transform.empty())
            return;

        gtl::span<program_pin_data const> programs = context.read<in_program_pin>();
        gtl::span<program_pin_data const> shadow_programs = context.read<in_shadow_program_pin>();
        for (std::size_t index = 0; index < programs.size(); ++index)
        {
            program_pin_data const& program = programs[index];
            if (!program.m_program.is_loaded() || !enabled[index < enabled.size() ? index : 0uz])
                continue;

            render_sys->get().add_mesh_draw_request(transform[index < transform.size() ? index : 0uz], program, shadow_programs[index < shadow_programs.size() ? index : 0uz], m_mesh);
        }
    }

    void render_mesh_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        serializer.process("mesh", m_mesh);
    }

    gcore::node::pin_descriptors render_mesh_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_enable_pin::get("enable"),
            in_transform_pin::get("transform"),
            in_program_pin::get("program"), 
            in_shadow_program_pin::get("shadow_program"),
        };
        return { inputs, {} };
    }
}

