//
// Created by thaqi on 12/9/2021.
//

#ifndef A4_PHYSICSSYSTEM_H
#define A4_PHYSICSSYSTEM_H

#include "../System.h"
#include "../../components/default/default.h"
class PhysicsSystem final: public System_base {
    bool update() override{
        float dt = DeltaTime();
        for(const auto& et : Entities){
            auto& pos = ComponentData.getComponentData<AGE_COMPONENTS::Position>(et);
            auto& vel = ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(et);
            auto& acc = ComponentData.getComponentData<AGE_COMPONENTS::Acceleration>(et);
            auto v0 = vel.v;
            vel.v += acc.a*(dt);
            pos.p += (vel.v + v0)*0.5*dt;
        }
        return true;
    }
};


#endif //A4_PHYSICSSYSTEM_H
