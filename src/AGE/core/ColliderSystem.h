//
// Created by thaqi on 12/14/2021.
//

#ifndef A4_COLLIDERSYSTEM_H
#define A4_COLLIDERSYSTEM_H

#include <set>
#include <utility>

#include "EntityManager.h"
#include "components/componentManager.h"

class ColliderSystem {
    using ColliderData = std::pair<EntityID, EntityID>;
    componentManager* ctx;
    EntityManager* entityManager;
    std::set<ColliderData> mCollidedEntities;
public:
    ColliderSystem(componentManager* ctx, EntityManager* entityManager) :
    ctx{ctx},
    entityManager{entityManager},
    mCollidedEntities()
    {

    }
    const std::set<ColliderData>& detectCollisions();
    void init();
    inline void resetCollisions(){
        mCollidedEntities.clear();
    }

private:
    template<class T>
    const T& clamp(const T& x, const T& upper, const T& lower) {
        return std::min(upper, std::max(x, lower));
    }
};


#endif //A4_COLLIDERSYSTEM_H
