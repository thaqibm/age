//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_DEFAULTSYSTEMS_H
#define A4_DEFAULTSYSTEMS_H


#include "PhysicsSystem.h"
#include "dynamicPhysics.h"
#include "colliderPhysics.h"

#include "../../AGEManager.h"
namespace AGE_SYSTEMS{
    void addPhysicsSystem(AGEManager& mgr){
        mgr.safe_addComponent<AGE_COMPONENTS::Velocity>();
        mgr.safe_addComponent<AGE_COMPONENTS::Acceleration>();
        mgr.registerSystem<PhysicsSystem>();
        mgr.SetSystemTraits<PhysicsSystem, AGE_COMPONENTS::Position, AGE_COMPONENTS::Velocity, AGE_COMPONENTS::Acceleration>();
    }

    void addDynamicPhysics(AGEManager& mgr){
        mgr.safe_addComponent<AGE_COMPONENTS::dynamicPosition>();
        mgr.registerSystem<DynamicPhysics>();
        mgr.SetSystemTraits<DynamicPhysics, AGE_COMPONENTS::dynamicPosition>();
    }

    void addColliderPhysics(AGEManager& mgr){
        assert(mgr.hasSystem<PhysicsSystem>());
        mgr.safe_addComponent<AGE_COMPONENTS::Velocity>();
        mgr.safe_addComponent<AGE_COMPONENTS::Mass>();
        mgr.registerSystem<ColliderPhysicsSystem>();
        mgr.SetSystemTraits<ColliderPhysicsSystem, AGE_COMPONENTS::Mass, AGE_COMPONENTS::Velocity>();
    }
}


#endif //A4_DEFAULTSYSTEMS_H
