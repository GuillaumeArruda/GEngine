#include "stdafx.h"

#include "gcore/script/node_data_type.h"
#include "gcore/resource_handle.h"
#include "gcore/script/node_factory.h"
#include "gcore/script/nodes/comparison_nodes.h"
#include "gcore/script/nodes/select_node.h"
#include "gcore/script/nodes/get_resource_node.h"

#include "grender/render_script_registration.h"

#include "grender/resources/texture.h"
#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"

#include "grender/nodes/configure_program_node.h"
#include "grender/nodes/render_mesh_node.h"

namespace grender
{
    void register_node_data_type()
    {
        gcore::node_data_type_registry& registry = gcore::node_data_type_registry::get();
        registry.register_type<gcore::resource_handle<grender::texture>>("Texture");
        registry.register_type<gcore::resource_handle<grender::mesh_resource>>("Mesh");
        registry.register_type<program_pin_data>("Program");
    }

    void register_node_type()
    {
        gcore::node_factory& factory = gcore::node_factory::get();

        GCORE_REGISTER_SELECT_NODE(gcore::resource_handle<grender::texture>, "Texture");
        GCORE_REGISTER_EQUALITY_NODES(gcore::resource_handle<grender::texture>, "Texture");

        GCORE_REGISTER_SELECT_NODE(gcore::resource_handle<grender::mesh_resource>, "Mesh");
        GCORE_REGISTER_EQUALITY_NODES(gcore::resource_handle<grender::mesh_resource>, "Mesh");

        factory.register_type<grender::configure_program_node>("grender::configure_program_node", "Configure Render Program");
        factory.register_type<grender::render_mesh_node>("grender::render_mesh_node", "Render Mesh");

        factory.register_type<gcore::get_resource_node<mesh_resource>>("gcore::get_resource_node<grender::mesh_resource>", "Get Resource (Mesh)");
        factory.register_type<gcore::get_resource_node<texture>>("gcore::get_resource_node<grender::texture>", "Get Resource (Texture)");

        factory.register_conversion<gcore::resource_handle<grender::texture>, gcore::resource_handle<gcore::resource>>("gcore::conversion_node<gcore::resource_handle<grender::texture>, gcore::resource_handle<gcore::resource>>");
        factory.register_conversion<gcore::resource_handle<gcore::resource>, gcore::resource_handle<grender::texture>>("gcore::conversion_node<gcore::resource_handle<gcore::resource>, gcore::resource_handle<grender::texture>>");

        factory.register_conversion<gcore::resource_handle<grender::mesh_resource>, gcore::resource_handle<gcore::resource>>("gcore::conversion_node<gcore::resource_handle<grender::mesh_resource>, gcore::resource_handle<gcore::resource>>");
        factory.register_conversion<gcore::resource_handle<gcore::resource>, gcore::resource_handle<grender::mesh_resource>>("gcore::conversion_node<gcore::resource_handle<gcore::resource>, gcore::resource_handle<grender::mesh_resource>>");
    }

}
