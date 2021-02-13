#include "stdafx.h"

#include <string>

#include "gcore/core_script_registration.h"
#include "gcore/resource_handle.h"
#include "gcore/script/node_data_type.h"
#include "gcore/script/node_factory.h"
#include "gcore/script/nodes/comparison_nodes.h"
#include "gcore/script/nodes/arithmetic_nodes.h"
#include "gcore/script/nodes//constant_node.h"

#include "gserializer/gmath_serialization.h"

#include "gtl/uuid.h"

#include "gmath/units.h"



namespace gcore
{
    void register_node_data_type()
    {
        node_data_type_registry& registry = gcore::node_data_type_registry::get();

        registry.register_type<int>("Integer");
        registry.register_type<float>("Float");
        registry.register_type<bool>("Bool");


        registry.register_type<std::string>("String");
        registry.register_type<gtl::uuid>("UUID");
        registry.register_type<gcore::resource_handle<resource>>("Resource");

        registry.register_type<glm::vec2>("Vec2");
        registry.register_type<glm::vec3>("Vec3");
        registry.register_type<glm::vec4>("Vec4");
        registry.register_type<glm::ivec2>("IVec2");
        registry.register_type<glm::ivec3>("IVec3");
        registry.register_type<glm::ivec4>("IVec4");
        registry.register_type<glm::mat4>("Matrix4");
        registry.register_type<glm::mat3>("Matrix3");
        registry.register_type<gmath::degree>("Degrees");
    }

    void register_node_type()
    {
        node_factory& factory = node_factory::get();
        GCORE_REGISTER_COMPARISON_NODES(int, "Integer");
        GCORE_REGISTER_ARITHMETIC_NODES(int, "Integer");
        GCORE_REGISTER_COMPARISON_NODES(float, "Float");
        GCORE_REGISTER_ARITHMETIC_NODES(float, "Float");
        GCORE_REGISTER_COMPARISON_NODES(bool, "Bool");
        GCORE_REGISTER_COMPARISON_NODES(std::string, "String");
        GCORE_REGISTER_COMPARISON_NODES(gtl::uuid, "UUID");
        GCORE_REGISTER_COMPARISON_NODES(gmath::degree, "Degrees");
        GCORE_REGISTER_ARITHMETIC_NODES(gmath::degree, "Degrees");

        factory.register_type<gcore::constant_node>("gcore::constant_node", "Constant");
    }
}