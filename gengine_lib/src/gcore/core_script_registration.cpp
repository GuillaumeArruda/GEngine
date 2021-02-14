#include "stdafx.h"

#include <string>

#include "gcore/core_script_registration.h"
#include "gcore/resource_handle.h"
#include "gcore/script/node_data_type.h"
#include "gcore/script/node_factory.h"
#include "gcore/script/nodes/comparison_nodes.h"
#include "gcore/script/nodes/arithmetic_nodes.h"
#include "gcore/script/nodes//constant_node.h"
#include "gcore/script/nodes/bool_operation_node.h"
#include "gcore/script/nodes/conversion_node.h"
#include "gcore/script/nodes/execution_nodes.h"
#include "gcore/script/nodes/print_node.h"
#include "gcore/script/nodes/select_node.h"
#include "gcore/script/nodes/random_nodes.h"

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

        registry.register_type<execution_pin_data>("Execute");
    }

    void register_node_type()
    {
        node_factory& factory = node_factory::get();
        GCORE_REGISTER_COMPARISON_NODES(int, "Integer");
        GCORE_REGISTER_ARITHMETIC_NODES(int, "Integer");
        factory.register_type<select_node<int>>("gcore::select_node<int>", "Select (Integer)");
        
        GCORE_REGISTER_COMPARISON_NODES(float, "Float");
        GCORE_REGISTER_ARITHMETIC_NODES(float, "Float");
        factory.register_type<select_node<float>>("gcore::select_node<float>", "Select (Float)");

        GCORE_REGISTER_COMPARISON_NODES(bool, "Bool");
        factory.register_type<select_node<bool>>("gcore::select_node<bool>", "Select (Bool)");

        GCORE_REGISTER_COMPARISON_NODES(std::string, "String");
        factory.register_type<select_node<std::string>>("gcore::select_node<std::string>", "Select (String)");
        
        GCORE_REGISTER_COMPARISON_NODES(gtl::uuid, "UUID");
        factory.register_type<select_node<gtl::uuid>>("gcore::select_node<gtl::uuid>", "Select (UUID)");
        
        GCORE_REGISTER_COMPARISON_NODES(gmath::degree, "Degrees");
        GCORE_REGISTER_ARITHMETIC_NODES(gmath::degree, "Degrees");
        factory.register_type<select_node<gmath::degree>>("gcore::select_node<gmath::degree>", "Select (gmath::degree)");
        

        factory.register_type<gcore::constant_node>("gcore::constant_node", "Constant");
        factory.register_type<gcore::and_node>("gcore::and_node", "And");
        factory.register_type<gcore::or_node>("gcore::or_node", "Or");
        factory.register_type<gcore::not_node>("gcore::not_node", "Not");

        factory.register_type<print_node>("gcore::print_node", "Print");
        factory.register_type<root_execution_node>("gcore::root_execution_node", "Root Execute");
        factory.register_type<sequence_execution_node>("gcore::sequence_execution_node", "Execute Sequence");
        factory.register_type<select_execution_node>("gcore::select_execution_node", "Select Execution");

        factory.register_type<random_integer_range_node>("gcore::random_integer_range_node", "Random Integer In Range");

        factory.register_conversion<int, float>("gcore::conversion_node<int, float>");
        factory.register_conversion<float, int>("gcore::conversion_node<float, int>"); 
        factory.register_conversion<float, gmath::degree>("gcore::conversion_node<float, gmath::degree>");
        factory.register_conversion<gmath::degree, float>("gcore::conversion_node<gmath::degree, float>");
    }
}