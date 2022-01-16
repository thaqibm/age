//
// Created by thaqi on 12/15/2021.
//


#include "../System.h"
#include "../../components/default/default.h"
#include "../../util/quadraticSolver.h"
#include <set>
#include "colliderPhysics.h"



bool ColliderPhysicsSystem::update() {
    std::set<std::pair<EntityID , EntityID>> updated;
    for(const auto& et1 : Entities){
        for(const auto& et2 : Entities){
            if(et1 == et2){
                continue ;
            }

            if(updated.find({et1,et2}) != updated.end() || updated.find({et2,et1}) != updated.end()){
                continue;
            }

            if(Collisions.didCollide(et1, et2)){
                auto& mass1 = ComponentData.getComponentData<AGE_COMPONENTS::Mass>(et1);
                auto& mass2 = ComponentData.getComponentData<AGE_COMPONENTS::Mass>(et2);
                auto& v1 =  ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(et1);
                auto& v2 =  ComponentData.getComponentData<AGE_COMPONENTS::Velocity>(et2);

                /**
                v1.v = {-10,-10};
                v2.v = {-10,-10};
                 **/

                updated.insert({et1, et2});

            }
        }
    }
    return true;
}
