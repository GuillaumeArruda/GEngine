#pragma once

#include <memory>

#include "gcore/system.h"

#include "gphys/debug_drawer.h"

#include "gtl/callbacks.h"

#include "gcore/entity_registry.h"

class btBroadphaseInterface;
class btCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btCollisionShape;
class btRigidBody;
struct btDefaultMotionState;

namespace gcore
{
    struct world;
    struct transform_component;
}

namespace gphys
{
    struct physic_component;
    struct physic_system : gcore::system
    {
        physic_system();
        ~physic_system();

        physic_system(physic_system&&) noexcept;
        physic_system& operator=(physic_system&&) noexcept;

        physic_system(physic_system const&) = delete;
        physic_system& operator=(physic_system const&) = delete;

        void connect_to_world(gcore::world& world) override;

        void update(gcore::world& world);

        void on_physic_entity_removed(std::tuple<gcore::entity, gcore::transform_component*, physic_component*> removed_entity);

        std::unique_ptr<btBroadphaseInterface> m_broadphase;
        std::unique_ptr<btCollisionConfiguration> m_collision_configuration;
        std::unique_ptr<btCollisionDispatcher> m_collision_dispatcher;
        std::unique_ptr<btSequentialImpulseConstraintSolver> m_constraint_solver;
        std::unique_ptr<btDiscreteDynamicsWorld> m_phys_world;

        gtl::callback_id m_on_removed_callback_id = 0;

        debug_drawer m_debug_drawer;
    };
}