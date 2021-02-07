#include "stdafx.h"

#include "gcore/script/node_data_type.h"
#include "gcore/resource_handle.h"

#include "gphys/phys_script_registration.h"

#include "gphys/resources/collision_shape.h"

namespace gphys
{
    void register_node_data_type()
    {
        gcore::node_data_type_registry& registry = gcore::node_data_type_registry::get();
        registry.register_type<gcore::resource_handle<gphys::collision_shape>>("Collision Shape");
    }
}

