#include <vector>
#include <cassert>

#include "gcore/world.h"

#include "gserializer/serializers/json_serializer.h"

struct component2 : gcore::component {
    GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
};

struct component3 : gcore::component 
{
    GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
};

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(component2);
GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(component3);

#include <iostream>

int main()
{
    gcore::world world;
    auto& registry = world.get_entity_registry();
    
    {
        gserializer::json_read_serializer seri("entities.json");
        seri.process("entity_registry", registry);
    }

    auto new_entity = registry.create_entity();
    std::vector<std::unique_ptr<gcore::component>> comps;
    comps.push_back(std::make_unique<component2>());
    comps.push_back(std::make_unique<component3>());
    registry.add_components(new_entity, comps);

    {
        gserializer::json_write_serializer seri;
        seri.process("entity_registry", registry);
        seri.write_to_file("entities.json");
    }

    auto view = registry.get_view<component2, component3>();
    for (auto& [comp2, comp3] : view)
    {
        std::cout << "Allo\n";
    }

    return 0;
}