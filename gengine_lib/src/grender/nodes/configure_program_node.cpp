#include "stdafx.h"

#include "grender/nodes/configure_program_node.h"

#include "gtl/crc32.h"

namespace grender
{
    void configure_program_node::execute(gcore::node_context& context) const
    {
        program_pin_data& pin_data = context.create_array<out_pin>(1)[0];
        pin_data.m_program = m_program;
        if (!pin_data.m_program.is_loaded())
        {
            context.create_array<out_pin>(1);
            return;
        }

        pin_data.m_state.reconcile(m_program->get_default_state());
        for (std::size_t i = 0; i < m_descriptors.size(); ++i)
        {
            set_state_at_index(context, pin_data.m_state, i);
        }
    }

    void configure_program_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        serializer.process("program", m_program);
    }

    gcore::node::pin_descriptors configure_program_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const output[] = {
            out_pin::get("program")
        };

        if (m_program.is_loaded())
        {
            program_uniform_state const& state = m_program->get_default_state();

            m_descriptors.clear();
            m_descriptors.reserve(state.get_uniform_number());
            for (std::size_t i = 0; i < state.get_uniform_number(); ++i)
            {
                gcore::pin_descriptor pin
                {
                    state.get_name_at_index(i).c_str(),
                    static_cast<int>(i),
                    static_cast<int>(gtl::compute_crc(state.get_name_at_index(i).c_str())),
                    get_variant_type_id(state, i),
                    gcore::pin_type::input,
                    false
                };
                m_descriptors.push_back(pin);
            }
        }


        return { m_descriptors, output };
    }

    void configure_program_node::set_state_at_index(gcore::node_context& context, program_uniform_state& state, std::size_t index) const
    {
        switch (state.get_type_at_index(index))
        {
        case uniform_variant::type::glbool:         state.set_uniform(static_cast<GLint>(index), context.read<bool>(index)[0]); break;
        case uniform_variant::type::glint:          state.set_uniform(static_cast<GLint>(index), context.read<int>(index)[0]); break;
        case uniform_variant::type::glfloat:        state.set_uniform(static_cast<GLint>(index), context.read<float>(index)[0]); break;
        case uniform_variant::type::bvec2:          state.set_uniform(static_cast<GLint>(index), context.read<glm::bvec2>(index)[0]); break;
        case uniform_variant::type::bvec3:          state.set_uniform(static_cast<GLint>(index), context.read<glm::bvec3>(index)[0]); break;
        case uniform_variant::type::bvec4:          state.set_uniform(static_cast<GLint>(index), context.read<glm::bvec4>(index)[0]); break;
        case uniform_variant::type::ivec2:          state.set_uniform(static_cast<GLint>(index), context.read<glm::ivec2>(index)[0]); break;
        case uniform_variant::type::ivec3:          state.set_uniform(static_cast<GLint>(index), context.read<glm::ivec3>(index)[0]); break;
        case uniform_variant::type::ivec4:          state.set_uniform(static_cast<GLint>(index), context.read<glm::ivec4>(index)[0]); break;
        case uniform_variant::type::vec2:           state.set_uniform(static_cast<GLint>(index), context.read<glm::vec2>(index)[0]); break;
        case uniform_variant::type::vec3:           state.set_uniform(static_cast<GLint>(index), context.read<glm::vec3>(index)[0]); break;
        case uniform_variant::type::vec4:           state.set_uniform(static_cast<GLint>(index), context.read<glm::vec4>(index)[0]); break;
        case uniform_variant::type::mat33:          state.set_uniform(static_cast<GLint>(index), context.read<glm::mat3>(index)[0]); break;
        case uniform_variant::type::mat44:          state.set_uniform(static_cast<GLint>(index), context.read<glm::mat4>(index)[0]); break;
        case uniform_variant::type::sampler_1d:     state.set_uniform(static_cast<GLint>(index), uniform_variant(context.read<gcore::resource_handle<texture>>(index)[0], uniform_variant::type::sampler_1d)); break;
        case uniform_variant::type::sampler_2d:     state.set_uniform(static_cast<GLint>(index), uniform_variant(context.read<gcore::resource_handle<texture>>(index)[0], uniform_variant::type::sampler_2d)); break;
        case uniform_variant::type::sampler_3d:     state.set_uniform(static_cast<GLint>(index), uniform_variant(context.read<gcore::resource_handle<texture>>(index)[0], uniform_variant::type::sampler_3d)); break;
        case uniform_variant::type::sampler_cube:   state.set_uniform(static_cast<GLint>(index), uniform_variant(context.read<gcore::resource_handle<texture>>(index)[0], uniform_variant::type::sampler_cube)); break;
        default:
            assert(false); // Unsupported data type
        }
    }

    gcore::node_data_type::id_type configure_program_node::get_variant_type_id(program_uniform_state const& state, std::size_t index) const
    {
        gcore::node_data_type_registry const& registry = gcore::node_data_type_registry::get();
        switch (state.get_type_at_index(index))
        {
        case uniform_variant::type::glbool:      return registry.get_type_id<bool>();
        case uniform_variant::type::glint:       return registry.get_type_id<int>();
        case uniform_variant::type::glfloat:     return registry.get_type_id<float>();
        case uniform_variant::type::bvec2:       return registry.get_type_id<glm::bvec2>();
        case uniform_variant::type::bvec3:       return registry.get_type_id<glm::bvec3>();
        case uniform_variant::type::bvec4:       return registry.get_type_id<glm::bvec4>();
        case uniform_variant::type::ivec2:       return registry.get_type_id<glm::ivec2>();
        case uniform_variant::type::ivec3:       return registry.get_type_id<glm::ivec3>();
        case uniform_variant::type::ivec4:       return registry.get_type_id<glm::ivec4>();
        case uniform_variant::type::vec2:        return registry.get_type_id<glm::vec2>();
        case uniform_variant::type::vec3:        return registry.get_type_id<glm::vec3>();
        case uniform_variant::type::vec4:        return registry.get_type_id<glm::vec4>();
        case uniform_variant::type::mat33:       return registry.get_type_id<glm::mat3>();
        case uniform_variant::type::mat44:       return registry.get_type_id<glm::mat4>();
        case uniform_variant::type::sampler_1d:
        case uniform_variant::type::sampler_2d:
        case uniform_variant::type::sampler_3d:
        case uniform_variant::type::sampler_cube:
            return registry.get_type_id<gcore::resource_handle<texture>>();
        default:
            assert(false); // Unsupported data type
        }
        return gcore::node_data_type::invalid_id;
    }

}
