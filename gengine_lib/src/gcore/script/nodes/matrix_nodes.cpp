#include <stdafx.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "gcore/script/nodes/matrix_nodes.h"
#include "gmath/units.h"

namespace gcore
{
    void make_mat4_node::execute_on_element(node_context&, glm::vec3 const& translation, glm::vec3 const& rotation, glm::vec3 const& scale, glm::mat4& out) const
    {
        using namespace gmath;
        glm::vec3 const rotation_radian = glm::vec3(radian(degree(rotation.x)), radian(degree(rotation.y)), radian(degree(rotation.z)));
        glm::quat const rotation_quat = glm::quat(rotation);
        
        out = glm::mat4(1.f);
        out = glm::translate(out, translation);
        out = glm::mat4(rotation_quat) * out;
        out = glm::scale(out, scale);
    }

    node::pin_descriptors make_mat4_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin_1::get("translation"),
            super::in_pin_2::get("rotation"),
            super::in_pin_3::get("scale"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("matrix"),
        };
        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }

    void break_mat4_node::execute(node_context& context) const
    {
        gtl::span<glm::mat4 const> inputs = context.read<in_matrix_pin>();
        gtl::span<glm::vec3> translations = context.create_array<out_translation_pin>(inputs.size());
        gtl::span<glm::vec3> rotations = context.create_array<out_rotation_pin>(inputs.size());
        gtl::span<glm::vec3> scales = context.create_array<out_scale_pin>(inputs.size());

        for (std::size_t i = 0; i < inputs.size(); ++i)
        {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::quat rotation;
            glm::decompose(inputs[i], scales[i], rotation, translations[i], skew, perspective);
            rotations[i] = glm::eulerAngles(rotation);
        }
    }

    node::pin_descriptors break_mat4_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            in_matrix_pin::get("matrix")
        };
        static pin_descriptor const output_descriptors[] = {
            out_translation_pin::get("translation"),
            out_rotation_pin::get("rotation"),
            out_scale_pin::get("scale"),
        };

        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }
}