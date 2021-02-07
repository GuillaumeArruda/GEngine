#include "stdafx.h"

#include "gcore/script/node_data_type.h"
#include "gcore/resource_handle.h"

#include "grender/render_script_registration.h"

#include "grender/resources/texture.h"
#include "grender/resources/mesh_resource.h"

namespace grender
{
    void register_node_datat_type()
    {
        gcore::node_data_type_registry& registry = gcore::node_data_type_registry::get();
        registry.register_type<gcore::resource_handle<grender::texture>>("Texture");
        registry.register_type<gcore::resource_handle<grender::mesh_resource>>("Mesh");
    }

}
