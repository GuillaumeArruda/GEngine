#include <vector>
#include <cassert>

#include "gcore/world.h"

struct component2 : gcore::component {};

struct component3 : gcore::component {};

#include <iostream>

int main()
{
    gcore::world world;
    auto& registry = world.get_entity_registry();
    std::vector<std::unique_ptr<gcore::component>> comp;
   
    comp.push_back(std::make_unique<component2>());
    comp.push_back(std::make_unique<component3>());
    auto entity = registry.create_entity();
    registry.add_components(entity, comp);

    comp.push_back(std::make_unique<component2>());
    comp.push_back(std::make_unique<component3>());
    auto entity2 = registry.create_entity();
    registry.add_components(entity2, comp);

    auto s = entity2.to_string();
    auto entity3 = gtl::uuid::from_string(s);

    assert(entity2 == entity3);

    auto view = registry.get_view<component2, component3>();
    for (auto& [comp2, comp3] : view)
    {
        std::cout << "ALLO\n";
    }

    registry.remove_component<component2>(entity2);

    for (auto& [comp2, comp3] : view)
    {
        std::cout << "ALLO2\n";
    }

    return 0;
}