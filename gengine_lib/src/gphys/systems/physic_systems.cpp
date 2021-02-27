#include "stdafx.h"
#include "gphys/systems/physic_systems.h"

#include <bullet/btBulletDynamicsCommon.h>

#include "gcore/world.h"
#include "gcore/components/transform_component.h"
#include "gcore/console.h"

#include "grender/systems/debug_render_system.h"

#include "gphys/components/physic_component.h"
#include "gphys/utils.h"

namespace gphys
{
    GCORE_REGISTER_CONSOLE_VALUE("physic.display", bool, physic_display, false);

    physic_system::physic_system()
    {
        m_broadphase = std::make_unique<btDbvtBroadphase>();
        m_collision_configuration = std::make_unique<btDefaultCollisionConfiguration>();
        m_collision_dispatcher = std::make_unique<btCollisionDispatcher>(m_collision_configuration.get());
        m_constraint_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        m_phys_world = std::make_unique<btDiscreteDynamicsWorld>(m_collision_dispatcher.get(), m_broadphase.get(), m_constraint_solver.get(), m_collision_configuration.get());
        m_phys_world->setGravity(btVector3(0.f, -9.81f, 0.f));

        m_phys_world->setDebugDrawer(&m_debug_drawer);
    }

    physic_system::~physic_system() = default;
    physic_system::physic_system(physic_system&&) noexcept = default;
    physic_system& physic_system::operator=(physic_system&&) noexcept = default;
    
    void physic_system::connect_to_world(gcore::world& world)
    {
        auto view = world.get_entity_registry().get_view<gcore::transform_component, physic_component>();
        m_on_removed_callback_id = view.add_on_removed_callback([this](std::tuple<gcore::entity, gcore::transform_component*, physic_component*> info) { this->on_physic_entity_removed(info); });
        m_on_added_callback_id = view.add_on_added_callback([this](std::tuple<gcore::entity, gcore::transform_component*, physic_component*> info) { this->on_physic_entity_added(info); });

        m_debug_drawer.m_debug_render = world.get_system_registry().get_system<grender::debug_render_system>();
    }

    void physic_system::update(gcore::world& world)
    {
        float const delta_time = gcore::time::to_float(world.get_time().get_delta_time());
        m_phys_world->stepSimulation(delta_time);
        if(physic_display)
            m_phys_world->debugDrawWorld();
    }

    void physic_system::on_physic_entity_added(std::tuple<gcore::entity, gcore::transform_component*, physic_component*>& added_entity)
    {
        physic_component* phys_comp = std::get<physic_component*>(added_entity);
        if (phys_comp->m_phys_shape.is_loaded())
        {
            gcore::transform_component* transform = std::get<gcore::transform_component*>(added_entity);
            btVector3 fall_inertia = btVector3(0.f, 0.f, 0.f);
            phys_comp->m_phys_shape->get_shape()->calculateLocalInertia(phys_comp->m_mass, fall_inertia);

            btRigidBody::btRigidBodyConstructionInfo constructionInfo(phys_comp->m_mass, &phys_comp->m_motion_state, phys_comp->m_phys_shape->get_shape().get(), fall_inertia);
            phys_comp->m_motion_state.m_transform = transform;
            phys_comp->m_motion_state.m_offset = glm::vec3(0.f, 1.f, 0.f);
            constructionInfo.m_startWorldTransform = to_bullet(transform->m_transform);
            phys_comp->m_rigid_body = std::make_unique<btRigidBody>(constructionInfo);
            m_phys_world->addRigidBody(phys_comp->m_rigid_body.get());
        }
    }

    void physic_system::on_physic_entity_removed(std::tuple<gcore::entity, gcore::transform_component*, physic_component*>& removed_entity)
    {
        if (auto rigid_body = std::get<physic_component*>(removed_entity)->m_rigid_body.get())
        {
            m_phys_world->removeRigidBody(rigid_body);
        }
    }
}


