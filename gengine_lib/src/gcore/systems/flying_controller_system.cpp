#include "stdafx.h"
#include "gcore/systems/flying_controller_system.h"

#include <glm/gtx/euler_angles.hpp>

#include "gcore/world.h"
#include "gcore/components/transform_component.h"
#include "gcore/components/input_component.h"
#include "gcore/components/flying_controller_component.h"
#include "gcore/spaces.h"

#include "gmath/direction.h"
#include "gmath/vector.h"
#include "gmath/utils.h"

#include <iostream>

#include <optick/optick.h>

namespace gcore
{
    void flying_controller_system::update(world& world)
    {
        OPTICK_EVENT();
        using gmath::vector;
        using gmath::direction;
        float const dt = time::to_float(world.get_time().get_delta_time());
        auto view = world.get_entity_registry().get_view<transform_component, input_component, flying_controller_component>();
        for (auto& [entity, transform, input, controller] : view)
        {
            glm::vec2 const delta_mouse = (glm::vec2(input->m_mouse_positions[0] - input->m_mouse_positions[1]))
                * dt * static_cast<float>(gmath::radian(controller->m_rotation_speed)) 
                * static_cast<float>(input->m_mouse_key_state[gtl::to_underlying(mouse_key::button_1)] == key_state::pressed);

            controller->m_pitch += delta_mouse[1];
            controller->m_yaw += delta_mouse[0];
            glm::quat const pitch(glm::vec3(controller->m_pitch, 0.f, 0.f));
            glm::quat const yaw(glm::vec3(0.f, controller->m_yaw, 0.f));
            glm::quat const new_rot = yaw * pitch;
            glm::vec3 const position = transform->m_transform[3];
            transform->m_transform = glm::mat4(new_rot);
            transform->m_transform[3] = glm::vec4(position,1.f);

            glm::vec3 direction_vector =
                  (direction<model_space>::up() * float(input->m_keybord_state[gtl::to_underlying(controller->m_up)] == key_state::pressed))
                - (direction<model_space>::up() * float(input->m_keybord_state[gtl::to_underlying(controller->m_down)] == key_state::pressed))
                + (direction<model_space>::right() * float(input->m_keybord_state[gtl::to_underlying(controller->m_right)] == key_state::pressed))
                - (direction<model_space>::right() * float(input->m_keybord_state[gtl::to_underlying(controller->m_left)] == key_state::pressed))
                + (direction<model_space>::forward() * float(input->m_keybord_state[gtl::to_underlying(controller->m_forward)] == key_state::pressed))
                - (direction<model_space>::forward() * float(input->m_keybord_state[gtl::to_underlying(controller->m_backward)] == key_state::pressed));

            if (gmath::within_epsilon(glm::length(direction_vector), 0.f))
            {
                continue;
            }
            
            direction_vector = glm::normalize(direction_vector);
            float const speed = controller->m_speed * dt * (1 + controller->m_boost_factor * (input->m_keybord_state[gtl::to_underlying(controller->m_boost_key)] == key_state::pressed));
            transform->m_transform[3] -= transform->m_transform * glm::vec4(direction_vector * speed, 0.f);
        }
    }
}

