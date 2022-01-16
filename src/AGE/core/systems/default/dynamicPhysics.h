//
// Created by thaqi on 12/15/2021.
//

#ifndef A4_DYNAMICPHYSICS_H
#define A4_DYNAMICPHYSICS_H

#include "../System.h"
#include "../../components/default/default.h"
class DynamicPhysics final: public System_base {
    bool update() override{
        float dt = DeltaTime();
        totalTime += dt;
        for(const auto& et : Entities){
            auto& pos = ComponentData.getComponentData<AGE_COMPONENTS::Position>(et);
            const auto& [pt]= ComponentData.getComponentData<AGE_COMPONENTS::dynamicPosition>(et);
            pos.p = pt(pos.p, totalTime);
        }

        return true;
    }

private:
    float totalTime;
};

#endif //A4_DYNAMICPHYSICS_H
